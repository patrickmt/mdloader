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

make

This will create a build/ directory with the compiled executable.  
Run ./build/mdloader to test.

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
To detect connected devices ready for programming:

mdloader --list

Assume for example the listing included a device at port name THE_PORT

To write firmware to the device:

mdloader -p THE_PORT -D new_firmware.bin

To read firmware from the device:

mdloader -p THE_PORT -U read_firmware.bin --addr 0x4000 --size 0x10000

Test mode may be used to test operations, just use the -t or --test switch.  
Test mode also allows viewing of binary data from a read instead of writing to a file.

You may also use the --restart switch to boot the keyboard into operating mode.

## Troubleshooting

Linux: User may need to be added to group dialout to access programming port
