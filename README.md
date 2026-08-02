[![latest release](https://img.shields.io/badge/latest%20release-v0.3-green.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)
[![latest dev](https://img.shields.io/badge/latest%20dev-v0.4--beta-orange.svg?style=flat-square)](https://github.com/GramThanos/WakeOnLAN/releases/latest)

# WakeOnLAN
A simple C program that sends a magic packet to wake up a PC over the network. It sends the so-called *magic packet* to the target's network card, instructing it to power on. [[Download]](https://github.com/GramThanos/WakeOnLAN/releases)

> **Requirements:** Wake-on-LAN must be enabled in the target machine's BIOS/UEFI and network adapter settings, and the sender must be on the same broadcast domain (LAN) as the target.

**Contents**
- [For Users](#for-users) — download, install, use, uninstall
- [For Developers](#for-developers) — build from source, project layout

___

## For Users

### Usage
```bash
WakeOnLAN <mac address> [<broadcast address>] [<interface>]
```
- `<mac address>` — MAC of the target, format `xx:xx:xx:xx:xx:xx` (required).
- `<broadcast address>` — broadcast address to send to (optional, default `255.255.255.255`).
- `<interface>` — source interface to send from (optional; **not supported on Windows**).

**Examples (Linux / macOS)**
```bash
# wake a single host
WakeOnLAN 00:11:22:33:44:55 192.168.1.255 eth0

# wake every MAC listed in a file (one per line)
WakeOnLAN -f ./macs-list.txt
```

**Examples (Windows)**
```powershell
# run from the folder containing the executable
# wake a single host (the interface parameter is not supported on Windows)
.\WakeOnLAN.exe 00:11:22:33:44:55 192.168.1.255

# wake every MAC listed in a file (one per line)
.\WakeOnLAN.exe -f .\macs-list.txt
```

where the MAC list file has one MAC address per line:
```
00:1A:2B:3C:4D:5E
00:11:22:33:44:55
12:34:56:78:90:AB
```

### Download

| Platform | v0.3 | v0.4-beta |
| ------------ | ------------- | ------------- |
| Windows | [x86](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_windows_x86.zip) | TBA |
| Linux | [x64](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_linux_x64.zip) | TBA |
| Raspberry Pi | [arm](https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_raspberrypi.zip) | TBA |

### Quick install (x64)

Copy-paste download, extract and install for the two most common platforms.

**Linux x64** (installs system-wide, needs sudo):
```bash
curl -LO https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_linux_x64.zip
unzip WakeOnLAN_v0.3_linux_x64.zip
sudo install -m 755 WakeOnLAN /usr/local/bin/WakeOnLAN
```

**Windows x64** (installs for the current user, no admin needed) — from PowerShell:
```powershell
# v0.3 ships an x86 build, which runs on 64-bit Windows; a native x64 build arrives with v0.4-beta
Invoke-WebRequest -Uri "https://github.com/GramThanos/WakeOnLAN/releases/download/v0.3/WakeOnLAN_v0.3_windows_x86.zip" -OutFile WakeOnLAN.zip
Expand-Archive -Force WakeOnLAN.zip -DestinationPath .
New-Item -ItemType Directory -Force "$env:LOCALAPPDATA\Programs\WakeOnLAN"
Copy-Item WakeOnLAN.exe "$env:LOCALAPPDATA\Programs\WakeOnLAN\"
[Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path","User") + ";$env:LOCALAPPDATA\Programs\WakeOnLAN", "User")
```

> Open a new terminal afterwards so the updated `PATH` takes effect.

### Install

The tool is a single self-contained executable. You can run it straight from the extracted folder, or install it so it's available from anywhere. Choose **system-wide** if you have admin rights and want it available to every user, or **local user** if you don't.

#### System-wide (requires sudo / administrator)

**Linux / macOS** — install into `/usr/local/bin` (already on every user's `PATH`):
```bash
sudo install -m 755 WakeOnLAN /usr/local/bin/WakeOnLAN
```

**Windows** — from an **elevated** PowerShell (Run as administrator):
```powershell
New-Item -ItemType Directory -Force "$env:ProgramFiles\WakeOnLAN"
Copy-Item WakeOnLAN.exe "$env:ProgramFiles\WakeOnLAN\"
[Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path","Machine") + ";$env:ProgramFiles\WakeOnLAN", "Machine")
```

#### Local user (no sudo / administrator)

**Linux / macOS** — install into `~/.local/bin`:
```bash
mkdir -p ~/.local/bin
install -m 755 WakeOnLAN ~/.local/bin/WakeOnLAN
# if the command isn't found, add ~/.local/bin to your PATH (e.g. in ~/.bashrc or ~/.zshrc):
export PATH="$HOME/.local/bin:$PATH"
```

**Windows** — from a normal PowerShell:
```powershell
New-Item -ItemType Directory -Force "$env:LOCALAPPDATA\Programs\WakeOnLAN"
Copy-Item WakeOnLAN.exe "$env:LOCALAPPDATA\Programs\WakeOnLAN\"
[Environment]::SetEnvironmentVariable("Path", [Environment]::GetEnvironmentVariable("Path","User") + ";$env:LOCALAPPDATA\Programs\WakeOnLAN", "User")
```

> After changing `PATH` on Windows, open a new terminal for it to take effect.

### Uninstall

Mirror whichever install you used.

#### System-wide (requires sudo / administrator)

**Linux / macOS**
```bash
sudo rm /usr/local/bin/WakeOnLAN
```

**Windows** — from an **elevated** PowerShell:
```powershell
Remove-Item -Recurse "$env:ProgramFiles\WakeOnLAN"
[Environment]::SetEnvironmentVariable("Path", (([Environment]::GetEnvironmentVariable("Path","Machine") -split ';' | Where-Object { $_ -ne "$env:ProgramFiles\WakeOnLAN" }) -join ';'), "Machine")
```

#### Local user (no sudo / administrator)

**Linux / macOS**
```bash
rm ~/.local/bin/WakeOnLAN
```

**Windows** — from a normal PowerShell:
```powershell
Remove-Item -Recurse "$env:LOCALAPPDATA\Programs\WakeOnLAN"
[Environment]::SetEnvironmentVariable("Path", (([Environment]::GetEnvironmentVariable("Path","User") -split ';' | Where-Object { $_ -ne "$env:LOCALAPPDATA\Programs\WakeOnLAN" }) -join ';'), "User")
```

___

## For Developers

### Project layout

File | Purpose
---- | -------
`WakeOnLAN.c` | The entire program — magic-packet construction, sockets, CLI parsing. **Change here for any feature or fix.**
`CMakeLists.txt` | CMake build configuration.
`Makefile` | Minimal GNU make build (Linux/macOS).
`docker-compose.yml` | One cross-compilation service per target platform.
`build.sh` / `build.cmd` | Run all Docker cross-builds, output to `build/`.
`macs-list.txt` | Example MAC-address list for `-f`.

It's a single-file C program (C99, no external dependencies beyond the platform socket library), so almost all changes happen in `WakeOnLAN.c`.

### Build from source (local tools)

**Linux — GCC**
```bash
gcc WakeOnLAN.c -o WakeOnLAN
```

**Windows — MinGW**
```bash
gcc WakeOnLAN.c -o WakeOnLAN.exe -lws2_32
```

**Windows — MSVC** (from a Visual Studio Native Tools Command Prompt)
```bash
cl WakeOnLAN.c
```

**GNU make** (Linux/macOS)
```bash
make          # build
make clean    # remove the binary
```

**CMake** (Linux/macOS/Windows)
```bash
cmake -B build
cmake --build build
```

### Build all platforms (Docker)

With Docker installed, build binaries for every supported target at once. Output goes to the `build/` directory.

```bash
./build.sh     # Linux/macOS
build.cmd      # Windows
```

This produces binaries for Linux (x64, x86, arm64), Windows (x64, x86, arm64) and macOS (x64, arm64).

___

### Feedback

For any problems, [open an issue](https://github.com/GramThanos/WakeOnLAN/issues).

___

### License

This project is under [The MIT license](https://opensource.org/licenses/MIT).
I do appreciate attribution.

Copyright (c) 2024-2026 Grammatopoulos Athanasios-Vasileios

___

[![GramThanos](https://avatars2.githubusercontent.com/u/14858959?s=42&v=4)](https://github.com/GramThanos)
[![DinoDevs](https://avatars1.githubusercontent.com/u/17518066?s=42&v=4)](https://github.com/DinoDevs)
