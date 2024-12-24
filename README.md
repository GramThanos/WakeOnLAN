[![latest release](https://img.shields.io/badge/latest%20release-v0.3-green.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
[![latest dev](https://img.shields.io/badge/latest%20dev-v1.0-orange.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
![Windows build](https://img.shields.io/badge/window%20build-pass-blue.svg?style=flat-square)
![Linux build](https://img.shields.io/badge/linux%20build-pass-blue.svg?style=flat-square)
![Mac build](https://img.shields.io/badge/mac%20build-pass-blue.svg?style=flat-square)

# WakeOnLAN
A simple C program that sends a magic packet

You can use this program to wake up a PC over the network. It sends the so-called magic packet to the network card of the target PC, instructing it to open the PC. [[Download]](https://github.com/GramThanos/WakeOnLAN/releases)

___

### Usage
```bash
./WakeOnLAN <mac address>[ <broadcast address>][ <interface>]
```
The first parameter is the mac address of the target (usually your network card's mac address).
The second parameter is optional and defines the broadcast address to send the packet.
The third parameter is optional and defines the source interface to send the packet from (not for Windows).

Example
```bash
./WakeOnLAN 00:11:22:33:44:55 192.168.1.255 eth0
```
or, from a file with MAC addresses inside:
```bash
./WakeOnLAN -f macs-list.txt
```
where `macs-list.txt` has one MAC address per line:
```
00:1A:2B:3C:4D:5E
00:11:22:33:44:55
12:34:56:78:90:AB
.
.
.
```
___

### Pre-compiled Binaries (Latest Release)

Platform | Links v0.3
------------ | -------------
Windows | [x86](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_windows_x86.zip)
Linux | [x64](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_linux_x64.zip)
Raspberry Pi | [arm](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_raspberrypi.zip)

___

### Compile from source
For Linux, you can compile the source using GCC
```bash
gcc WakeOnLAN.c -o WakeOnLAN
```

For Windows, you can compile the source using MinGW
```bash
gcc WakeOnLAN.c -o WakeOnLAN.exe -lwsock32
```

Or Open up a Visual Studio Native Tools Command Prompt, navigate to your source directory
```bash
cl WakeOnLAN.c -o WakeOnLAN.exe
```

### Makefile
GNU make can be used in order to compile the sources on Linux/Mac:
```bash
make
```

To clean everything:
```bash
make clean
```

### CMake
CMake can also be used on Linux/Mac:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
___

### Feedback

For any problems, you may [open an issue](https://github.com/GramThanos/WakeOnLAN/issues)

Also, you can [send me an email](mailto:gramthanos@gmail.com)

___

### License

This project is under [The MIT license](https://opensource.org/licenses/MIT).
I do appreciate attribution.

Copyright (c) 2024 Grammatopoulos Athanasios-Vasileios

___

[![GramThanos](https://avatars2.githubusercontent.com/u/14858959?s=42&v=4)](https://github.com/GramThanos)
[![DinoDevs](https://avatars1.githubusercontent.com/u/17518066?s=42&v=4)](https://github.com/DinoDevs)

