# WaffleOS
A Unix-like open source operating system written in NASM and C, with the goal of demystifying how operating systems work.

<img src="https://github.com/user-attachments/assets/0553e678-e8b8-4853-ad1e-6e97e162c5fd" alt="drawing" width="256"/> <img src="https://github.com/user-attachments/assets/4fd84e48-94bf-4ded-829d-f6726b7cd01d" alt="CLI" width="256"/>

![Hack Club arcade 2024 finalist](https://img.shields.io/badge/Hack%20Club%20arcade%202024-showcase%20finalist-gold?logo=https%3A%2F%2Fassets.hackclub.com%2Ficon-rounded.png&logoSize=auto&labelColor=orange)
[![CodeFactor](https://www.codefactor.io/repository/github/alandoescs/waffleos/badge)](https://www.codefactor.io/repository/github/alandoescs/waffleos)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/AlanDoesCS/WaffleOS)
![GitHub commit activity](https://img.shields.io/github/commit-activity/t/AlanDoesCS/WaffleOS)
![GitHub contributors](https://img.shields.io/github/contributors/AlanDoesCS/WaffleOS)
![GitHub Repo stars](https://img.shields.io/github/stars/AlanDoesCS/WaffleOS)
![GitHub forks](https://img.shields.io/github/forks/AlanDoesCS/WaffleOS)

---
# Table of contents
1) [Current Features](#features)
2) [Installation Guide](#installation)
3) [Command List](#commands)

---
## Features
- Bootloader
    - Boots into 32 bit protected mode, and loads the C kernel
- Disk driver
    - A simple ATA PIO mode disk read/write driver
- CLI
    - VGA text mode to display information
- FAT12 Support (WIP)
- Interrupt descriptor table
- Memory management
    - allows memory management with malloc(), calloc() and free()
- Programmable Interval Timer
    - allow precise time delays
- HTTP client (WIP)
    - RFC 3986 compliant URI parsing

---
## Installation
*WaffleOS is currently very early stages, so it may be prone to errors*

### (For users) Virtual Machine (QEMU)
*This guide requires you to be on a Linux based system, if on Windows, then it is recommended to install the "Windows Subsystem for Linux" (WSL)*
*To install the WSL, simply run the command:* `wsl --install` *and follow the on-screen prompts, before restarting your system.*

1) `sudo apt update`
2) `sudo apt install qemu-system-i386`
   - Download the latest OS image from the releases page, or from builds/bin/...
3) `qemu-system-i386 -drive format=raw,file=PATH/TO/os-image.bin`
4) Enjoy your emulated WaffleOS experience! 🎉🎉

### (For developers) Virtual Machine (QEMU)
*This guide requires you to be on a Linux based system, if on Windows, then it is recommended to install the "Windows Subsystem for Linux" (WSL)*
*To install the WSL, simply run the command:* `wsl --install` *and follow the on-screen prompts, before restarting your system.*

1) First you will need to clone this repository before you can proceed with the installation
2) Locate the installation script, "`setup-gcc-debian.sh`" and run `chmod +x setup-gcc-debian.sh` from the same directory in your terminal, before using `./setup-gcc-debian.sh`
    - This will install QEMU, NASM, GCC and a bunch of other things needing for compiling the OS
OR
    - Follow this guide from OSDev: https://wiki.osdev.org/GCC_Cross-Compiler
3) To verify your installation:
    - You should see something like this after running the setup
      ![Terminal output](https://github.com/AlanDoesCS/WaffleOS/assets/95879019/72d8dc06-bd04-4357-9046-aeb43f707513)
    - Then run:
   ```shell
   qemu-system-i386 --version
   nasm -v
   gcc --version
   ```
4) Next, add the new directory ("/usr/local/i386elfgcc/bin") to the path variable:
    - Run `vim ~/.bashrc`
    - Add "`export PATH="$PATH:/usr/local/i386elfgcc/bin"`" to the end of the file
    - Add "`export PATH="$PATH:/usr/local/i386elfld/bin"`" to the end of the file
    - Save and exit (`:x`)
5) Restart your system
6) Locate `source/run.sh` and run `chmod +x run.sh` then `./run.sh`
7) Enjoy your emulated WaffleOS experience! 🎉🎉

### x86 based device

1) *Guide in progress*

---
## Commands
*This list is subject to change*

- `help`
  - Prints a list of available commands
- `clear`
  - Clears the screen
- `shutdown` (not yet implemented, but will use ACPI)
  - Shuts down the system

### Fun
- `hello`
  - Prints "Hello, World!" to the screen
- `waffle`
    - Prints "WaffleOS" to the screen, along with a waffle in ascii
---

## WIP
These features have NOT YET BEEN IMPLEMENTED (hence WIP)

### WIP Commands
- Filesystem:
    - `ls`
    - `cd`
    - `mkdir`
    - `mv`
    - `touch`
    - `rm`
    - `cat`
- Miscellaneous:
    - `webster`
- Graphics
    - `graphics -enable/disable` (enables or disables vesa graphics)

### BOCHS

For linux mint:
```shell
./configure --enable-smp \
              --enable-cpu-level=6 \
              --enable-all-optimizations \
              --enable-x86-64 \
              --enable-pci \
              --enable-vmx \
              --enable-debugger \
              --enable-disasm \
              --enable-debugger-gui \
              --enable-logging \
              --enable-fpu \
              --enable-3dnow \
              --enable-sb16=dummy \
              --enable-cdrom \
              --enable-x86-debugger \
              --enable-iodebug \
              --disable-plugins \
              --disable-docbook \
              --with-term --with-sdl2
```