# Chip-8 Emulator

CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker made on his 1802 Microprocessor. The simplicity of CHIP-8, and its long history and popularity, has ensured that CHIP-8 emulators and programs are still being made to this day.

## Description

This Chip-8 emulator was implemented in C++, with SDL as the graphics library to draw graphics to the screen, and read keypresses. All of the instruction sets have been implemented along with audio output.

## Getting Started

### Dependencies

* SDL graphics library
* MinGW (for windows)
* CMake for (Unix)

### Installation

* #### Cloning and Running Locally for Windows

  * Clone the project in a directory:
    * ```git clone https://github.com/ahmadadil950/Chip8-Emulator.git```
  * Go in to project directory:
    * ```cd chip8```
  * Run the project with the makefile, if you have mingw-make:
    * ``` make or mingw32-make.exe ```
  * Then using the following the command to run a ROM
    * ``` .\chip8.exe C:(Project Download Location)\chip8\ROMS\TETRIS ```

* #### Cloning and Running Locally for Unix/MacOs
  * Since this requires CMake and SDL2 run:
    * ```$ sudo apt-get install cmake libsdl2-dev```
  * Clone the project in a directory:
    * ```git clone https://github.com/ahmadadil950/Chip8-Emulator.git```
  * Go in to project directory:
    * ```cd chip8```
  * Compile:

    ```
    mkdir build
    cd build
    cmake ..
    make
    ```
  * Run:
    ``` ./chip8 <ROM file>```

## References

* [High-level guide to making a CHIP-8 emulator by Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator)
* <https://en.wikipedia.org/wiki/CHIP-8#CHIP-8_today>
* <https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference>
