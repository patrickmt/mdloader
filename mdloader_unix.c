/*
 *  Copyright (C) 2018  Massdrop Inc.
 *
 *  This file is part of Massdrop Loader.
 *
 *  Massdrop Loader is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Massdrop Loader is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Massdrop Loader.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mdloader_common.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC == 1
#define EXAMPLE_PORT "/dev/cu.usbmodem14221"
#define PORT_SEARCH_STRING "cu.usbmodem"
#endif
#else
#define EXAMPLE_PORT "/dev/ttyACM0"
#define PORT_SEARCH_STRING "ttyACM"
#endif

int gport; //Port of device

#define SLEEP_W_MIN     1000    //Sleep (us) per write call
#define SLEEP_W_CHR     20      //Sleep (us) per byte written
#define WRITE_SIZE      250     //Maximum bytes to write per call
#define READ_SIZE       250     //Maximum bytes to read per call
#define READ_RETRIES    10      //Maximum read retries before fail

void print_com_example(void)
{
    printf("Example: -p " EXAMPLE_PORT "\n");
}

//Read data from device
//Must check read_error for a read error after return
//Return unsigned word from memory location
int read_data(int addr, int readsize)
{
    read_error = 1; //Set read error flag as default

    int readdata = 0;
    char wbuf[] = "!XXXXXXXX,#"; //Largest buffer needed
    long ret;

    if (readsize == 1)
        sprintf(wbuf,"%c%02x,%c",CMD_READ_ADDR_8,addr,CMD_END);
    else if (readsize == 2)
        sprintf(wbuf,"%c%04x,%c",CMD_READ_ADDR_16,addr,CMD_END);
    else if (readsize == 4)
        sprintf(wbuf,"%c%08x,%c",CMD_READ_ADDR_32,addr,CMD_END);
    else
    {
        if (verbose) printf("Error: Invalid read size given (%i)\n",readsize);
        return 0;
    }

    if (verbose > 0) printf("Write: [%s]\n",wbuf);

    tcflush(gport,TCIOFLUSH); //Flush any remaining data from a bad read

    long writelen = strlen(wbuf);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    if (ret != writelen)
    {
        if (verbose) printf("Error writing %ld bytes [%ld](%s)\n",writelen,ret,strerror(errno));
        return 0;
    }

    if ((ret = read(gport, &readdata, readsize)) == -1)
    {
        if (verbose) printf("Error reading port [%i][%ld](%s)\n",readsize,ret,strerror(errno));
        return 0;
    }

    if (ret != readsize)
    {
        if (verbose) printf("Error reading %i bytes! [%ld]\n",readsize,ret);
        return 0;
    }

    read_error = 0; //Clear read error flag

    return readdata;
}

//Write data to device
//Return 1 on success, 0 on failure
int write_data(int addr, int writesize, int data)
{
    if (check_bootloader_write_attempt(addr)) return 0; //Prevent writes to bootloader section

    char wbuf[] = "!XXXXXXXX,XXXXXXXX#"; //Largest buffer needed
    long ret;

    if (writesize == 1)
        sprintf(wbuf,"%c%08x,%02x%c",CMD_WRITE_ADDR_8,addr,data,CMD_END);
    else if (writesize == 2)
        sprintf(wbuf,"%c%08x,%04x%c",CMD_WRITE_ADDR_16,addr,data,CMD_END);
    else if (writesize == 4)
        sprintf(wbuf,"%c%08x,%08x%c",CMD_WRITE_ADDR_32,addr,data,CMD_END);
    else
    {
        if (verbose) printf("Error: Invalid write size given (%i)\n",writesize);
        return 0;
    }

    if (verbose) printf("Write %i bytes: [%s]\n",writesize,wbuf);

    long writelen = strlen(wbuf);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    if (ret != writelen)
    {
        if (verbose) printf("Error writing %i bytes! [%ld]\n",writesize,ret);
        return 0;
    }

    return 1;
}

//Jump to address and run
//Return 1 on success, 0 on failure
int goto_address(int addr)
{
    char wbuf[] = "!XXXXXXXX#";
    long ret;

    sprintf(wbuf,"%c%08x%c",CMD_GOTO_ADDR,addr,CMD_END);

    if (verbose) printf("Write: [%s]\n",wbuf);

    long writelen = strlen(wbuf);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    if (ret != writelen)
    {
        if (verbose) printf("Error writing goto address! [%ld]\n",ret);
        return 0;
    }

    return 1;
}

//Read data from device RAM
//Return pointer to buffer on success, NULL on failure
char *recv_file(int addr, int bytes)
{
    char wbuf[] = "!XXXXXXXX,XXXXXXXX#";
    char *data = NULL;

    data = (char *)calloc(bytes+1,sizeof(char));
    if (data == NULL)
    {
        printf("Error allocating read buffer memory!\n");
        return NULL;
    }

    char *pdata = data;
    long ret;
    int retries = READ_RETRIES;
    int readsize = READ_SIZE;

    //Constrain read size to buffer size
    if (initparams.argument.outputInit.bufferSize > 0 && initparams.argument.outputInit.bufferSize < readsize)
        readsize = initparams.argument.outputInit.bufferSize;

    while (bytes > 0)
    {
        if (readsize > bytes) readsize = bytes;

        sprintf(wbuf,"%c%08x,%08x%c",CMD_RECV_FILE,addr,readsize,CMD_END);
        if (verbose > 0) printf("Write: [%s]\n",wbuf);

        tcflush(gport,TCIOFLUSH); //Flush any remaining data from a bad read

        long writelen = strlen(wbuf);
        if ((ret = write(gport,wbuf,writelen)) == -1)
        {
            if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
            free(data);
            return NULL;
        }

        usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

        if ((ret = read(gport, pdata, readsize)) == -1)
        {
            if (verbose) printf("Error reading port [%i][%ld](%s)\n",readsize,ret,strerror(errno));
            free(data);
            return NULL;
        }

        if (ret != readsize)
        {
            if (verbose) printf("Error reading %i bytes! [%ld](%s)\n",readsize,ret,strerror(errno));

            if (retries <= 0)
            {
                if (verbose) printf("No retries remain!\n");
                free(data);
                return NULL;
            }

            retries--;

            if (verbose) printf("Retrying read... (%i)\n",READ_RETRIES - retries);

            continue; //Attempt read again
        }

        if (verbose > 0) printf("Recv: [%ld]\n",ret);

        retries = READ_RETRIES; //Reset retry limit

        bytes -= ret;  //Decrement remaining bytes
        addr += ret;   //Increment to next address
        pdata += ret;  //Increment pointer in recv buffer
    }

    return data;
}

//Write data to device
//Return 1 on sucess, 0 on failure
int send_file(int addr, int bytes, char *data)
{
    if (check_bootloader_write_attempt(addr)) return 0; //Prevent writes to bootloader section

    if (bytes < 1)
    {
        printf("Error: No data to send!\n");
        return 0;
    }

    char wbuf[] = "!XXXXXXXX,XXXXXXXX#";
    long ret;

    char *pdata = data;
    long writelen;
    int writesize = WRITE_SIZE;

    //Constrain write size to buffer size if it has been reported
    if (initparams.argument.outputInit.bufferSize > 0 && initparams.argument.outputInit.bufferSize < writesize)
        writesize = initparams.argument.outputInit.bufferSize;

    while (bytes > 0)
    {
        if (writesize > bytes) writesize = bytes;

        sprintf(wbuf,"%c%08x,%08x%c",CMD_SEND_FILE,addr,writesize,CMD_END);
        if (verbose > 0) printf("Write: [%s]\n",wbuf);

        writelen = strlen(wbuf);
        if ((ret = write(gport,wbuf,writelen)) == -1)
        {
            if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
            return 0;
        }

        usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

        if (ret != writelen)
        {
            if (verbose) printf("Error writing port [%s](%s)\n",wbuf,strerror(errno));
            return 0;
        }

        if (verbose > 0) printf("Write: %i bytes\n",writesize);
        if ((ret = write(gport,pdata,writesize)) == -1)
        {
            if (verbose) printf("Error writing port [%ld][%i](%s)\n",ret,writesize,strerror(errno));
            return 0;
        }

        usleep(SLEEP_W_MIN + SLEEP_W_CHR * writesize);

        if (ret != writesize)
        {
            printf("Error writing port [%ld][%i](%s)\n",ret,writesize,strerror(errno));
            return 0;
        }

        bytes -= ret;  //Decrement remaining bytes
        addr += ret;   //Increment to next address
        pdata += ret;  //Increment pointer in send buffer
    }

    return 1;
}

//Print bootloader version
//Return 1 on sucess, 0 on failure
int print_version(void)
{
    char wbuf[] = "!#";
    char readdata[128] = "";
    long ret;
    int readsize = sizeof(readdata) - 1;

    sprintf(wbuf,"%c%c",CMD_READ_VERSION,CMD_END);

    if (verbose > 0) printf("Write: [%s]\n",wbuf);

    long writelen = strlen(wbuf);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        if (verbose) printf("Version: Error writing port [%s](%s)\n",wbuf,strerror(errno));
        else printf("Version: Error retrieving!\n");
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    if ((ret = read(gport, &readdata, readsize)) == -1)
    {
        if (verbose) printf("Version: Error reading port [%i][%ld](%s)\n",readsize,ret,strerror(errno));
        else printf("Version: Error retrieving!\n");
        return 0;
    }

    while (readdata[strlen(readdata)-1] == '\n' || readdata[strlen(readdata)-1] == '\r') readdata[strlen(readdata)-1] = 0;

    printf("Version: %s\n",readdata);

    return 1;
}

//Set normal command mode
//Return 1 on sucess, 0 on failure
int set_normal_mode(void)
{
    if (verbose) printf("Setting normal mode... ");

    int retbuf = 0;
    char wbuf[] = "!#";
    long ret;

    sprintf(wbuf,"%c%c",CMD_SET_NORMAL_MODE,CMD_END);

    long writelen = strlen(wbuf);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        if (verbose) printf("Failed! (%s)\n",strerror(errno));
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    if (ret != writelen)
    {
        if (verbose) printf("Error writing %ld bytes! [%ld](%s)\n",writelen,ret,strerror(errno));
        return 0;
    }

    int readlen = 2;
    if ((ret = read(gport, &retbuf, readlen)) == -1)
    {
        if (verbose) printf("Error reading port [%i][%ld](%s)\n",readlen,ret,strerror(errno));
        return 0;
    }

    if (ret != readlen)
    {
        if (verbose) printf("Error reading %i bytes! [%ld][%04x](%s)\n",readlen,ret,retbuf,strerror(errno));
        return 0;
    }

    if ((retbuf & 0xFFFF) != 0x0D0A)
    {
        if (verbose) printf("Error: Incorrect response! [%ld][%04x](%s)\n",ret,retbuf,strerror(errno));
        return 0;
    }

    if (verbose) printf("Success!\n");

    return 1;
}

//Jump to loaded application
//Return 1 on sucess, 0 on failure
int jump_application(void)
{
    printf("Booting device... ");

    if (testmode)
    {
        printf("(test mode disables restart)\n");
        return 1;
    }

    char wbuf[] = "!#";
    long ret;
    long writelen = strlen(wbuf);
    sprintf(wbuf,"%c%c",CMD_LOAD_APP,CMD_END);
    if ((ret = write(gport,wbuf,writelen)) == -1)
    {
        printf("Failed! (%s)\n",strerror(errno));
        return 0;
    }

    usleep(SLEEP_W_MIN + SLEEP_W_CHR * (int)writelen);

    printf("Success!\n");
    return 1;
}

//Open port
//Return 1 on sucess, 0 on failure
int open_port(char *portname, char silent)
{
    if (!silent || verbose) printf("Opening port '%s'... ",portname);

    gport = open(portname,O_RDWR|O_NOCTTY);
    if (gport == -1)
    {
        if (!silent || verbose)
        {
            printf("Failed!");
            printf(" (%s)",strerror(errno));
            printf("\n");
        }

        return 0;
    }

    if (!silent || verbose) printf("Success!\n");
    return 1;
}

//Close port
//Return 1 on sucess, 0 on failure
int close_port(char silent)
{
    if (!silent || verbose) printf("Closing port... ");
    if (close(gport) == -1)
    {
        if (!silent || verbose) printf("Failed! (%s)\n",strerror(errno));
        return 0;
    }
    if (!silent || verbose) printf("Success!\n");
    return 1;
}

//Configure port
//Return 1 on sucess, 0 on failure
int config_port(void)
{
    if (verbose) printf("Configuring port... \n");

    struct termios tty;

    memset(&tty,0,sizeof(tty));

    if (verbose) printf("  Get config... ");
    if (tcgetattr(gport, &tty) != 0)
    {
        if (verbose) printf("Failed! (%s)\n",strerror(errno));
        return 0;
    }
    if (verbose) printf("Success!\n");

    cfsetspeed(&tty, (speed_t)B115200);

    cfmakeraw(&tty);

    tty.c_cflag = CS8 | CREAD | CLOCAL;
    tty.c_iflag = IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    if (verbose) printf("  Set config... ");
    if (tcsetattr(gport,TCSANOW,&tty) != 0)
    {
        if (verbose) printf("Failed! (%s)\n",strerror(errno));
        return 0;
    }
    if (verbose) printf("Success!\n");

    tcflush(gport,TCIOFLUSH); //Flush port

    return 1;
}

#define FNMAX 255

//List devices which communicate properly
void list_devices(void)
{
    char devdir[] = "/dev";
    DIR *pdev;

    pdev = opendir(devdir);
    if (pdev != NULL)
    {
        struct dirent *pdevfile;
        int portcount = 0;

        printf("Bootloader port listing\n");
        printf("-----------------------------\n");
        while ((pdevfile = readdir(pdev)) != NULL)
        {
            if (pdevfile->d_type == DT_CHR)
            {
                if (strstr(pdevfile->d_name,PORT_SEARCH_STRING) == pdevfile->d_name)
                {
                    char pathbuf[sizeof(devdir)+1+FNMAX+1] = "";
                    sprintf(pathbuf,"%s/%s",devdir,pdevfile->d_name);
                    if (test_port(pathbuf,TRUE))
                    {
                        if (test_mcu(TRUE))
                        {
                            printf("Device port: %s (%s)\n",pathbuf,mcu->name);
                            portcount++;
                        }
                        close_port(TRUE);
                    }
                }
            }
        }

        closedir(pdev);

        if (portcount == 0)
            printf("No devices found!\n");
    }
    else
        printf("Error: Could not open dev directory (%s)\n",strerror(errno));
}

