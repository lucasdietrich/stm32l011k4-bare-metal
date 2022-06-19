# Bare metal for `nucleo_l011k4` 

This project shows how to build, flash and debug a bare metal application on the `nucleo_l011k4` board.

Two build methods are available:
- `Makefile`
- `CMake`

## Get repositoy + prerequisites

Clone with : `git clone https://github.com/lucasdietrich/stm32l011k4-bare-metal.git --recurse-submodules`

Prerequisites:
- `cmake version 3.22.2`
- `arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)`
- `Open On-Chip Debugger 0.11.0`
- `GNU gdb (GDB) Fedora 12.1-1.fc36`

This project is fully integrated with Visual Studio Code, with extensions:
- `ms-vscode.cpptools`
- `marus25.cortex-debug`

## Build, flash, debug

Build using `cmake` :
- `cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain-arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build`

Build using Makefile :
- make --file=LegacyMakefile 

Flash the application to the board :
- `make flash`

Debug:
- Select `Target Debug` VS code debug confication
- `Ctrl + F5` in VS Code

![](./pics/linux-arm-debug.png)

Clean : 
- `make clean`

## Debug in QEMU using GDB

Run `make qemu`
Select `Qemu Debug` VS code debug configuration and then press `Ctrl + F5`

Important note: as only the core is emulated in QEMU following parts should be disabled:
- Startup code for RAM initialization
- Peripheral handling

## Note

Topics:
- startup code
- mixing assembly and c code
- newlibc
- linker script
- STM32L0Cube drivers:
  - `syslock`
  - `rcc`
  - `gpio`
  - `mco`
  - `uart`
- `Makefile` / `cmake`
- `debug`

---

## Sources

- Tutorials : 
  - [Bare Metal - From zero to blink](https://linuxembedded.fr/2021/02/bare-metal-from-zero-to-blink)
    - Tutorial github repo : https://github.com/joved-git/Bare_Metal_STM32
  - [CMake on STM32 | Episode 1: the beginning](https://dev.to/younup/cmake-on-stm32-the-beginning-3766)
  - [How to cross-compile for embedded with CMake like a champ](https://kubasejdak.com/how-to-cross-compile-for-embedded-with-cmake-like-a-champ)
  - Video tutorial : [STM32 Startup code, Bare metal - Part 3](https://www.youtube.com/watch?v=7stymN3eYw0)
- Others:
  - [ARM Cortex-M bare-metal example code in C++ and CMake build](https://github.com/cortexm/baremetal)
  - [LVC21-308 Essential ARM Cortex M Debugging with GDB](https://www.youtube.com/watch?v=QQcp8CPjkoY)

- Example of ARM toolchain .cmake files :
  - https://github.com/vpetrigo/arm-cmake-toolchains
    - Especially : https://github.com/vpetrigo/arm-cmake-toolchains/blob/master/arm-gcc-toolchain.cmake
  - https://github.com/jobroe/cmake-arm-embedded
    - Especially : https://github.com/jobroe/cmake-arm-embedded/blob/master/toolchain-arm-none-eabi.cmake
- [STM32Cube demo vscode](https://github.com/EmbeddedGeekYT/egSTM32/tree/vscode)

- [Others personnal notes](./personnal_notes.md)