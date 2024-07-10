# WaffleOS
An open source operating system written in NASM and C, with the goal of demystifying how operating systems work.

---
# Table of contents
1) [Current Features](#features)
2) [Installation Guide](#installation)

---
## Features
*WaffleOS is currently very early stages, so it will be prone to errors*

---
## Installation
### (For developers) Virtual Machine (QEMU)
*This guide requires you to be on a Linux based system, if on Windows, then it is recommended to install the "Windows Subsystem for Linux" (WSL)*
*To install the WSL, simply run the command:* `wsl --install` *and follow the on-screen prompts, before restarting your system.*

1) First you will need to clone this repository before you can proceed with the installation
2) Locate the installation script, "`setup-gcc-debian.sh`" and run `chmod +x setup-gcc-debian.sh` from the same directory in your terminal, before using `./setup-gcc-debian.sh`
    - This will install QEMU, NASM, GCC and a bunch of other things needing for compiling the OS
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
    - Run `vim .bashrc`
    - Add "`export PATH="$PATH:/usr/local/i386-elf-gcc/bin"`" to the end of the file
    - Add "`export PATH="$PATH:/usr/local/i386-elf-ld/bin"`" to the end of the file
    - Save and exit (`:x`)
5) Restart your system
6) Locate `source/run.sh` and run `chmod +x run.sh` then `./run.sh`
7) Enjoy your emulated WaffleOS experience! 🎉🎉

### (For users) Virtual Machine (QEMU)
*This guide requires you to be on a Linux based system, if on Windows, then it is recommended to install the "Windows Subsystem for Linux" (WSL)*
*To install the WSL, simply run the command:* `wsl --install` *and follow the on-screen prompts, before restarting your system.*

1) *Guide in progress*

### x86 based device

1) *Guide in progress*
