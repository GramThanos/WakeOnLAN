[![latest version](https://img.shields.io/badge/latest%20version-v0.3-green.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
![Windows build](https://img.shields.io/badge/window%20build-pass-blue.svg?style=flat-square)
![Linux build](https://img.shields.io/badge/linux%20build-pass-blue.svg?style=flat-square)
![Mac build](https://img.shields.io/badge/mac%20build-not--available-red.svg?style=flat-square)

# WakeOnLAN
A simple C program that sends a magic packet

You can use this program to wake up a PC over the network. It sends the so called magic packet to the network card of the target PC, instructing it to open the PC. [Download](https://github.com/GramThanos/WakeOnLAN/releases)

## Usage
```bash
./WakeOnLAN <mac address>[ <broadcast address>]
```
The first parameter is the mac address of the target (usually your's network card's mac address).
The second parameter is optional and defines the broadcast address to send the packet.

Example
```bash
./wakeonlan 00:11:22:33:44:55 192.168.1.255
```

## Compile from source
For Linux you can compile the source using GCC
```bash
gcc WakeOnLAN.c -o WakeOnLAN
```

For Windows you can compile the source using MinGW
```bash
gcc WakeOnLAN.c -o WakeOnLAN.exe -lwsock32
```


## Contact me

Contact us to leave us your feedback or to express your thoughts.

[Open an issue](https://github.com/GramThanos/WakeOnLAN/issues)

[Send me a mail](mailto:agrammatopoulos@isc.tuc.gr)



## Buy me a beer :beer:

I like beer, you can buy me one by donating to my Bitcoin wallet

My Bitcoin wallet address `173advnPpAgh4UDJXd2TFwTCgAppBKvqKF`



## License

This project is under [The MIT license](https://opensource.org/licenses/MIT).
We do although appreciate attribute.

Copyright (c) 2017 Grammatopoulos Athanasios-Vasileios
