### tl;dr

1. Follow the instructions here to download the executable for your operating system: https://github.com/Massdrop/mdloader/releases

2. Plug in your keyboard.

3. Press the reset button on your keyboard.

4. In your terminal, change to the directory where you downloaded the executable.

5. **Windows** - Run `mdloader_windows.exe --list`. Copy the port name, e.g. `/dev/ttyACM0`, `/dev/ttyS23`, `/dev/cu.usbmodem234411`.  
**Linux** -  Run `mdloader_linux --list`. Copy the port name as described above.  
**Mac** - Run `mdloader_mac --list`. If you downloaded with Mac Safari, run `mdloader_mac.dms --list`. Copy the port name as described above.

6. **Windows** - Run `mdloader_windows.exe --port PORT_NAME --download FILE_NAME --restart`. Replace "PORT_NAME" with the port name you copied in the previous step. Replace "FILE_NAME" with the filename of your compiled firmware.  
**Linux** - Run `mdloader_linux --port PORT_NAME --download FILE_NAME --restart`. Replace "PORT_NAME" and "FILE_NAME" in the command as instructed above.  
**Mac** - Run `mdloader_mac --port PORT_NAME --download FILE_NAME --restart`.  If you downloaded with Mac Safari, run `mdloader_mac.dms --port PORT_NAME --download FILE_NAME --restart`. Replace "PORT_NAME" and "FILE_NAME" in the command as instructed above.

7. Enjoy (important)

-----

# Massdrop Loader

Massdrop Loader is used to read firmware from and write firmware to Massdrop keyboards which utilize Microchip's SAM-BA bootloader, over the USB connection.

## Supported operating systems

Windows XP or greater (32-bit and 64-bit versions, USB Serial driver in drv_win folder)  
Linux x86 (32-bit and 64-bit versions)  
Mac OS X

## Supported devices

Massdrop keyboard's featuring Microchip's SAM-BA bootloader.

## Building

Enter mdloader directory where Makefile is located and excute:

`make`

This will create a build/ directory with the compiled executable.  
Run `./build/mdloader` to test.

## Usage
```
Usage: mdloader [options] ...
  -h --help                      Print this help message
  -v --verbose                   Print verbose messages
  -V --version                   Print version information
  -l --list                      Print valid attached devices for programming
  -p --port port                 Specify programming port
  -U --upload file               Read firmware from device into <file>
  -a --addr address              Read firmware starting from <address>
  -s --size size                 Read firmware size of <size>
  -D --download file             Write firmware from <file> into device
  -t --test                      Test mode (download/upload writes disabled, upload outputs data to stdout, restart disabled)
     --cols count                Hex listing column count <count> [8]
     --colw width                Hex listing column width <width> [4]
     --restart                   Restart device after successful programming
```

To detect connected keyboards ready for programming:

`mdloader --list`

If you do not see your keyboard listed, try pressing the reset button on your keyboard and try again.

Assume for example the listing included a device at port name `THE_PORT`

To write firmware to the device and restart it:

`mdloader --port THE_PORT --download new_firmware.bin --restart`

To read firmware from the device:

`mdloader --port THE_PORT --upload read_firmware.bin --addr 0x4000 --size 0x10000`

Test mode may be used to test operations, just use the -t or --test switch.  
Test mode also allows viewing of binary data from a read instead of writing to a file.

You may also use the --restart switch to boot the keyboard into operating mode.

## Troubleshooting

Linux: User may need to be added to group dialout to access programming port
