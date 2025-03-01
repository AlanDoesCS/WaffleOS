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
3) `qemu-system-i386 -drive format=raw,file=PATH/TO/main_floppy.img`
4) Enjoy your emulated WaffleOS experience! 🎉🎉

### (For developers) Virtual Machine (QEMU)
*This guide requires you to be on a Linux based system, if on Windows, then it is recommended to install the "Windows Subsystem for Linux" (WSL)*
*To install the WSL, simply run the command:* `wsl --install` *and follow the on-screen prompts, before restarting your system.*

1) First you will need to clone this repository before you can proceed with the installation
2) https://wiki.osdev.org/GCC_Cross-Compiler

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
- `cowsay <arg>`
  - Prints a cow which says the argument, or "WaffleOS!" if no argument is provided
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
    - `graphics -enable/disable` (enables or disables vbe graphics)

# Bochs

For linux mint, I recommend to use these configuration flags for bochs:
```shell
./configure --enable-smp \
              --enable-cpu-level=6 \
              --enable-all-optimizations \
              --enable-x86-64 \
              --enable-pci \
              --enable-vbe \
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

# References
Many thanks to the following resources for helping me to create WaffleOS, I would not have been able to do it without them:
```bibtex
@misc{nanobyte_os,
  author       = {Tiberiu C.},
  title        = {Nanobyte OS},
  year         = {2024},
  url          = {https://github.com/nanobyte-dev/nanobyte_os},
  note         = {Accessed: 2025-03-01}
}
@misc{osdev,
  author       = {{OSDev Community}},
  title        = {OSDev Wiki},
  year         = {2025},
  url          = {https://wiki.osdev.org/Main_Page},
  note         = {Accessed: 2025-03-01}
}
@misc{oslib,
  author       = {Luca Abeni and Gerardo Lamastra},
  title        = {The OSLib Project},
  year         = {2000},
  url          = {https://oslib.sourceforge.net/overview.html},
  note         = {Accessed: 2025-03-01}
}
```