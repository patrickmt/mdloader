/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2013, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef APPLET_H
#define APPLET_H

/*----------------------------------------------------------------------------
 *        Global definitions
 *----------------------------------------------------------------------------*/

/** Refers to the Version of SAM-BA */
#define SAM_BA_APPLETS_VERSION  "2.16"

/** Applet initialization command code.*/
#define APPLET_CMD_INIT              0x00
/** Applet full erase command code.*/
#define APPLET_CMD_FULL_ERASE        0x01
/** Applet write command code.*/
#define APPLET_CMD_WRITE             0x02
/** Applet read command code.*/
#define APPLET_CMD_READ              0x03
/** Applet read command code.*/
#define APPLET_CMD_LOCK              0x04
/** Applet read command code.*/
#define APPLET_CMD_UNLOCK            0x05
/** Applet set/clear GPNVM command code.*/
#define APPLET_CMD_GPNVM             0x06
/** Applet set security bit command code.*/
#define APPLET_CMD_SECURITY          0x07
/** Applet buffer erase command code.*/
#define APPLET_CMD_BUFFER_ERASE      0x08
/** Applet binary page command code for Dataflash.*/
#define APPLET_CMD_BINARY_PAGE       0x09
/** List Bad Blocks of a Nandflash*/
#define APPLET_CMD_LIST_BAD_BLOCKS   0x10
/** Tag a Nandflash Block*/
#define APPLET_CMD_TAG_BLOCK         0x11
/** Read the Unique ID bits (on SAM3)*/
#define APPLET_CMD_READ_UNIQUE_ID    0x12
/** Applet blocks erase command code. */
#define APPLET_CMD_ERASE_BLOCKS      0x13
/** Applet batch full erase command code. */
#define APPLET_CMD_BATCH_ERASE       0x14
/** Applet row erase command */
#define APPLET_CMD_ERASE_ROW         0x40
/** Applet read device ID command */
#define APPLET_CMD_READ_DEVICE_ID    0x41
/** Applet read lock bits command */
#define APPLET_CMD_READ_LOCKS        0x42
/** Applet read fuses command */
#define APPLET_CMD_READ_FUSES        0x43
/** Applet erase application section command */
#define APPLET_CMD_ERASE_APP         0x44
/** Applet information command */               //Massdrop Specific
#define APPLET_CMD_INFO              0xF0

/** Operation was successful.*/
#define APPLET_SUCCESS          0x00
/** Device unknown.*/
#define APPLET_DEV_UNKNOWN      0x01
/** Write operation failed.*/
#define APPLET_WRITE_FAIL       0x02
/** Read operation failed.*/
#define APPLET_READ_FAIL        0x03
/** Protect operation failed.*/
#define APPLET_PROTECT_FAIL     0x04
/** Unprotect operation failed.*/
#define APPLET_UNPROTECT_FAIL   0x05
/** Erase operation failed.*/
#define APPLET_ERASE_FAIL       0x06
/** No device defined in board.h*/
#define APPLET_NO_DEV           0x07
/** Read / write address is not aligned*/
#define APPLET_ALIGN_ERROR      0x08
/** Read / write found bad block*/
#define APPLET_BAD_BLOCK        0x09
/** Applet failure.*/
#define APPLET_FAIL             0x0f


/** Communication link identification*/
#define USB_COM_TYPE            0x00
#define DBGU_COM_TYPE           0x01
#define JTAG_COM_TYPE           0x02

/** \brief Structure for storing parameters for each command that can be
 *   performed by the applet. */
//Note: 9 x 32-bit word in size
typedef struct mailbox_s {

    /** Command send to the monitor to be executed. */
    uint32_t command;
    /** Returned status, updated at the end of the monitor execution.*/
    uint32_t status;

    /** Input Arguments in the argument area. */
    union {

        /** Input arguments for the Init command.*/
        struct {

            /** Communication link used.*/
            uint32_t comType;
            /** Trace level.*/
            uint32_t traceLevel;
            /** Memory Bank to write in.*/
            uint32_t bank;

        } inputInit;

        /** Output arguments for the Init command.*/
        struct {

            /** Memory size.*/
            uint32_t memorySize;
            /** Buffer address.*/
            uint32_t bufferAddress;
            /** Buffer size.*/
            uint32_t bufferSize;
            struct {
                /** Lock region size in byte.*/
                uint16_t lockRegionSize;
                /** Number of Lock Bits.*/
                uint16_t numbersLockBits;
            } memoryInfo;
            /** extended infos.*/
            uint32_t pageSize;
            uint32_t nbPages;
            uint32_t appStartPage;
        } outputInit;

        /** Input arguments for the Write command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputWrite;

        /** Output arguments for the Write command.*/
        struct {

            /** Bytes written.*/
            uint32_t bytesWritten;
        } outputWrite;

        /** Input arguments for the Read command.*/
        struct {

            /** Buffer address.*/
            uint32_t bufferAddr;
            /** Buffer size.*/
            uint32_t bufferSize;
            /** Memory offset.*/
            uint32_t memoryOffset;

        } inputRead;

        /** Output arguments for the Read command.*/
        struct {

            /** Bytes read.*/
            uint32_t bytesRead;

        } outputRead;

        /** Input arguments for the Full Erase command.*/
        /** NONE*/

         /** Input arguments for the Lock row command.*/
        struct {

            /** Row number to be lock.*/
            uint32_t row;

        } inputLock;

        /** Output arguments for the Lock row command.*/
        /** NONE*/

        /** Input arguments for the Unlock row command.*/
        struct {

            /** Row number to be unlock.*/
            uint32_t row;

        } inputUnlock;

        /** Output arguments for the Unlock row command.*/
        /** NONE*/

        /** Input arguments for the set security bit command.*/
        /** NONE*/

        /** Output arguments for the set security bit command.*/
        /** NONE*/

        /** Input arguments for the Read Locks command. */
        /** NONE */

        /** Output arguments for the Read Locks command. */
        struct {
            /** Buffer address. */
            uint32_t bufferAddr;
        } outputReadLocks;

        /** Input arguments for the Read Fuses command. */
        /** NONE */

        /** Output arguments for the Read Fuses command. */
        struct {
            /** Buffer address. */
            uint32_t bufferAddr;
        } outputReadFuses;

        /** Input arguments for the Read Unique SN command.*/
        struct {
            /** Buffer address.*/
            uint32_t bufferAddr;
        } inputReadUniqueID;

        /** Output arguments for the Read Unique SN command.*/
        /** NONE*/

        /** Input arguments for the Security command.*/
        struct {

            /** Activates*/
            uint32_t action;
        } inputSecurity;

        /** Output arguments for the Security command.*/
        /** NONE */

        /** Input arguments for the erase row command.*/
        struct {
            /** page.*/
            uint32_t row;
        } inputEraseRow;

        /** Output arguments for the  erase row command.*/
        /*NONE*/

        /** Input arguments for the erase app command */
        struct {
            /** Starting row number */
            uint32_t start_row;
            /** Ending row number */
            uint32_t end_row;
        } inputEraseApp;

        /** Output arguments for the erase app command */
        /** NONE */

        /** Input arguments for information command */  //Massdrop Specific
        /** NONE */

        /** Output arguments for information command */  //Massdrop Specific
        struct {
            /** Applet version number */
            uint16_t version_number;
        } outputInfo;
    } argument;
} mailbox_t;

#endif /* #ifndef APPLET_H */
