[![latest version](https://img.shields.io/badge/latest%20version-v0.3-green.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
![Windows build](https://img.shields.io/badge/window%20build-pass-blue.svg?style=flat-square)
![Linux build](https://img.shields.io/badge/linux%20build-pass-blue.svg?style=flat-square)
![Mac build](https://img.shields.io/badge/mac%20build-not--available-red.svg?style=flat-square)

# WakeOnLAN
A simple C program that sends a magic packet

You can use this program to wake up a PC over the network. It sends the so called magic packet to the network card of the target PC, instructing it to open the PC. [[Download]](https://github.com/GramThanos/WakeOnLAN/releases)

___

### Usage
```bash
./WakeOnLAN <mac address>[ <broadcast address>]
```
The first parameter is the mac address of the target (usually your's network card's mac address).
The second parameter is optional and defines the broadcast address to send the packet.

Example
```bash
./wakeonlan 00:11:22:33:44:55 192.168.1.255
```

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

___

### Contact me

Contact us to leave us your feedback or to express your thoughts.

[Open an issue](https://github.com/GramThanos/WakeOnLAN/issues)

[Send me a mail](mailto:agrammatopoulos@isc.tuc.gr)

___

### License

This project is under [The MIT license](https://opensource.org/licenses/MIT).
We do although appreciate attribute.

Copyright (c) 2017 Grammatopoulos Athanasios-Vasileios

___

[![GramThanos](https://avatars2.githubusercontent.com/u/14858959?s=42&v=4)](https://github.com/GramThanos)
[![DinoDevs](https://avatars1.githubusercontent.com/u/17518066?s=42&v=4)](https://github.com/DinoDevs)

