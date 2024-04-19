[![latest version](https://img.shields.io/badge/latest%20version-v0.3-green.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
![Windows build](https://img.shields.io/badge/window%20build-pass-blue.svg?style=flat-square)
![Linux build](https://img.shields.io/badge/linux%20build-pass-blue.svg?style=flat-square)
![Mac build](https://img.shields.io/badge/mac%20build-pass-blue.svg?style=flat-square)

# WakeOnLAN
A simple C program that sends a magic packet

You can use this program to wake up a PC over the network. It sends the so called magic packet to the network card of the target PC, instructing it to open the PC. [[Download]](https://github.com/GramThanos/WakeOnLAN/releases)

___

### Usage
```bash
./WakeOnLAN <mac address>[ <broadcast address>][ <interface>]
```
The first parameter is the mac address of the target (usually your's network card's mac address).
The second parameter is optional and defines the broadcast address to send the packet.
The third parameter is optional and defines the source interface to send the packet from it (not for windows).

Example
```bash
./WakeOnLAN 00:11:22:33:44:55 192.168.1.255 eth0
```

___

### Pre-compiled Binaries

Platform | Links
------------ | -------------
Windows | [x86](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_windows_x86.zip)
Linux | [x64](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_linux_x64.zip)
Raspberry Pi | [arm](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_raspberrypi.zip)

___

### Compile from source
For Linux you can compile the source using GCC
```bash
gcc WakeOnLAN.c -o WakeOnLAN
```

For Windows you can compile the source using MinGW
```bash
gcc WakeOnLAN.c -o WakeOnLAN.exe -lwsock32
```

### Makefile
GNU make can be used in order to compile the sources on Linux/Mac:
```bash
make
```
to clean everything:
```bash
make clean
```
___

### Feedback

For any problem you may [open an issue](https://github.com/GramThanos/WakeOnLAN/issues)

Also, you can [send me a mail](mailto:gramthanos@gmail.com)

___

### License

This project is under [The MIT license](https://opensource.org/licenses/MIT).
I do although appreciate attribute.

Copyright (c) 2019 Grammatopoulos Athanasios-Vasileios

___

[![GramThanos](https://avatars2.githubusercontent.com/u/14858959?s=42&v=4)](https://github.com/GramThanos)
[![DinoDevs](https://avatars1.githubusercontent.com/u/17518066?s=42&v=4)](https://github.com/DinoDevs)

