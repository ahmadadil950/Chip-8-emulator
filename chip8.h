/**
 * @file chip8.h
 * @author Ahmad Adil
 * @brief The CHIP8 Class which contains the Specifications:
 * 
 * Memory: Direct access upto 4 kilobytes of RAM
 * Display: 64 x 32 pixels, which are monochrome colors
 * Program Counter (PC): Points to the current instruction in memory
 * Index Register (I): Points at locations in memory
 * Stack: For 16-bit addresses, used to call subrotines/functions and return from them
 * Delay Timer: An 8-bit delay timer, decremented at rate of 60Hz until it reaches 0
 * Sound Timer: An 8-bit sound timer, has the same functions as the delay timer, but will give a beeping sound
 *              when not 0
 * General Purpose Register: Variable register, numbered 0 - F, V0 - VF
 *                           VF is used as the flag register, some instructions set it to either 1 or 0 based on some rule, 
 *                           for example using it as a carry flag
 *        
 * @version 0.1
 * @date 2023-01-08
 * 
 */


#ifndef CHIP8
#define CHIP8

#include <stdio.h>
#include <stdint.h>

/**
 * Chip 8 class that contains:
 * Memory
 * Display
 * Program Counter (PC)
 * Index Register (I)
 * Stack
 * Delay Timer
 * Sound Timer
 * V-Registers
 */
class chip_8
{
private:

    uint8_t V[16];  // general purpose registers
    uint8_t s_timer;    // sound timer
    uint8_t d_timer;    // delay timer

    uint8_t MEM[4096];  // Memory buffer

    uint16_t I;         // Index Register
    uint16_t PC;        // Program Counter
    uint16_t stack[16]; // Stack
    uint16_t SP;        // Stack Pointer

    uint16_t opcode;    // Current OP code

    void init();        // initalization function


public:

    uint8_t display[64*32]; // Graphics Buffer
    uint8_t key[16];        // Keypad

    bool drawflag;          // flag to indicate if a draw has occured
    bool playsound = false; // flag to indicate if sound needs to be played

    void emu_cycle();       // Emulate a cycle

    bool load(const char *file_path); // Load the ROM

    chip_8();           // constructor
    ~chip_8();          // deconstructor

};

#endif