#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include "time.h"
#include "chip8.h"

using namespace std; 


unsigned char fontset[80]
{
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip_8::chip_8() {}

chip_8::~chip_8() {}

// Initalization
void chip_8::init()
{
    // Clearing the stack, keypad, and v-registers
    for(int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        key[i] = 0;
        V[i] = 0;
    }


    PC = 0x200; // PC counter equal to 0x200
    opcode = 0; // Reseting OP CODE
    SP = 0;     // Reseting the stack pointer
    I = 0;      // Reset index register


    // Clearing the display
    for(int i = 0; i < 2048; ++i)
    {
        display[i] = 0;
    }

    // Clearing Memory
    for(int i = 0; i < 4096; ++i)
    {
        MEM[i] = 0;
    }

    // Load fonset into memory
    for(int i = 0; i < 80; ++i)
    {
        MEM[i] = fontset[i];
    }
    
    // initalize both sound and delay timers to 0
    s_timer = 0;
    d_timer = 0;

    
    srand(time(NULL));

}


bool chip_8::load(const char *file_path) // init and loading ROM into memory
{
    init();

    printf("Loading ROM: %s\n", file_path);

    FILE* rom = fopen(file_path, "rb"); // Opening the ROM file
    
    if(rom == NULL)
    {
        cout << "failed to open" << endl;
        return false;
    }

    // Getting ROM file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    // Allocating memory to store ROM file
    char* rom_buffer = (char*)malloc(sizeof(char)*rom_size);
    if(rom_buffer == NULL)
    {
        cout<<"failed to allocate memory for ROM" << endl;
        return false;
    }

    // Reading ROM file
    size_t result = fread(rom_buffer,sizeof(char),(size_t)rom_size,rom);
    if(result != rom_size)
    {
        cout<<"failed to to read rom"<<endl;
        return false;
    }

    // Copy ROM buffer to memory
    if((4096-512) > rom_size)
    {
        for(int i = 0; i < rom_size; ++i)
        {
            MEM[i+512] = (uint8_t)rom_buffer[i];
        }
    }

    else
    {
        cout<<"ROM too large for memory"<<endl;
        return false;
    }

    // free up memory
    fclose(rom);
    free(rom_buffer);

    return true;
}

/**
 * Beginning of emulation cycle
 * Where all OPCODE will be executed
 * 
 */
void chip_8::emu_cycle()
{
    /**
     * Fetching the opcode and masking the memory index with 0xFFF
     * resulting in a wrap-around, preventing a memory out of bounds issue
     * Implemented everywhere memory buffer is called
     */
    opcode = MEM[PC & 0xFFF] << 8 | MEM[(PC+1) & 0xFFF];

    switch (opcode & 0xF000)
    {
        //00E:
        case 0x0000:

        switch(opcode & 0x000F)
        {
        case 0x0000:                    // 00E0: Clear screen
        for(int i = 0; i<2048; ++i)
        {
            display[i] = 0;
        }

        drawflag = true;

        PC += 2;
        
        break;

    /**
     * 
     * 0EEE
     * return from subroutines
     * removing (“popping”) the last address from the stack and setting the PC to it.
     */
    case 0x000E:
    --SP;
    PC = stack[SP];
    PC += 2;
    break;

    default:
    printf("\nUnknown opcode: %.4X\n", opcode);
    exit(3);
    }
    break;

/**
 * 1NNN - Jump directly to address NNN
 * 
 */
case 0x1000:
PC = opcode & 0x0FFF;
break;

/**
 * 2NNN - Calls subroutine at NNN
 * 2NNN calls the subroutine at memory location NNN. Just like 1NNN.
 * set PC to NNN 
 * instruction should first push the current PC to the stack, so the subroutine can return later.
 */
case 0x2000:
stack[SP] = PC;
++SP;
PC = opcode & 0x0FFF;
break;

/**
 * Skips to the next instruction if VX equals NN
 * 3XNN
 */
case 0x3000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

/**
 * Skips to the next instruction if VX is NOT equal to NN
 * 4XNN
 */
case 0x4000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

/**
 * 5XY0
 * Skips to the next instruction if VX equals VY
 */
case 0x5000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

/**
 * 6XNN - Sets VX to NN
 */
case 0x6000:
V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
PC += 2;
break;

/**
 * 7XNN: Add
 * Add value NN to VX
 */
case 0x7000:
V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
PC += 2;
break;

/*8XY: Logical and arithmetic instructions*/
case 0x8000:
    switch (opcode & 0x000F)
    {
        // 8XY0: Sets VX to value of VY
        case 0x0000:
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00FF) >> 4]; // 8XY0 - Sets VX to the value of VY.
        PC+=2;
        break;

        // 8XY1: VX is set to the bitwise/binary logical disjunction (OR) of VX and VY. VY is not affected.
        case 0x0001:
        V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        // 8XY2: VX is set to the bitwise/binary logical disjunction (AND) of VX and VY. VY is not affected.
        case 0x0002:
        V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        // 8XY3: VX is set to the bitwise/binary exclusive OR (XOR) of VX and VY. VY is not affected.
        case 0x0003:
        V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        // 8XY4 (ADD): VX is set to the value of VX plus the value of VY. VY is not affected.
        case 0x0004:
        V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00FF) >> 4];
        if(V[(opcode & 0x00F0 >> 4)] > (0xFF - V[(opcode & 0x0F00) >> 8]))
        {
            V[0xF] = 1;
        }
        else
        {
            V[0xF] = 0;
        }
        PC+=2;
        break;

        // 8XY5 (Subtract): sets VX to the result of VX - VY. VF is set to 0 when
        //                  there's a borrow, and 1 when there isn't.
        case 0x0005:
        if(V[(opcode & 0x00F0 >> 4)] > (0xFF - V[(opcode & 0x0F00) >> 8]))
        {
            V[0xF] = 0;
        }
        else
        {
            V[0xF] = 1;
        }
        V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        //8XY6 (shift): Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
        case 0x0006:
        V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
        V[(opcode & 0x0F00) >> 8] >>=1;
        PC+=2;
        break;

        // 8XY7 (subtract): sets VX to the result of VY - VX.
        case 0x0007:
        if(V[(opcode & 0x00F0 >> 8)] > (0xFF - V[(opcode & 0x0F00) >> 4]))
        {
            V[0xF] = 0;
        }
        else
        {
            V[0xF] = 1;
        }
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00FF) >> 4] - V[(opcode & 0x0F00) >> 8];
        PC+=2;
        break;

        // 8XYE: Shifts VX Left by one. VF is set to the value of the least significant bit of VX before the shift.
        case 0x000E:
        V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
        V[(opcode & 0x0F00) >> 8] <<=1;
        PC+=2;
        break;

        default:
        printf("\nUnknown opcode: %.4X\n", opcode);
        exit(3);
    }
    break;

    // 9XY0 (skip): Skips to the next instruction if VX NOT equal VY
    case 0x9000:
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
    PC += 4;
    else
    PC += 2;
    break;

    // ANNN: Set index - sets the index register I to the value NNN.
    case 0xA000:
    I = opcode & 0x0FFF;
    PC+=2;
    break;

    // BNNN: Jump with offset. Jump to address NNN + V0
    case 0xB000:
    PC = (opcode & 0x0FFF) + V[0];
    break;

    // CXNN: Random - Sets VX to a random number, masked by NN. 
    case 0xC000:
    V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF+1)) & (opcode & 0x00FF);
    PC += 2;
    break;

    // DXYN: Display
    /**
     * It will draw an N pixels tall sprite from the memory location that the I index register is holding to the screen, 
     * at the horizontal X coordinate in VX and the Y coordinate in VY. 
     * All the pixels that are “on” in the sprite will flip the pixels on the screen that it is drawn to (from left to right, 
     * from most to least significant bit). If any pixels on the screen were turned “off” by this, the VF flag register is set to 1. 
     * Otherwise, it’s set to 0.
     * Each row of 8 pixels is read as bit-coded starting from memory location I
     * I doesnt change after execution
     */
    case 0xD000:
    {
        unsigned short X = V[(opcode & 0x0F00) >> 8];
        unsigned short Y = V[(opcode & 0x00F0) >> 4];
        unsigned short h = opcode & 0x000F;
        unsigned short p;

        V[0xF] = 0;

        for(int yLine = 0; yLine < h; yLine++)
        {
            p = MEM[(I + yLine) & 0xFFF];
            for(int xLine = 0; xLine < 8; xLine++)
            {
                if((p & (0x80 >> xLine)) != 0){
                if(display[(X+xLine + ((Y+yLine) * 64))] == 1)
                {
                    V[0xF] = 1;
                }
                display[X+xLine+((Y+yLine)*64)] ^= 1;
                }
            }
        }

        drawflag = true;
        PC+=2;
    } 
    break;

    // EX
    case 0xE000:
    switch(opcode & 0x00FF)
    {
        // EX9E (Skip if key): skip one instruction (increment PC by 2) if the key corresponding to the value in VX is pressed.
        case 0x009E:
        if (key[V[(opcode & 0x0F00) >> 8]] != 0)
        PC+=4;
        else
        PC+=2;
        break;

        // EXA1 (Skip if key): Skip if the key corresponding to the value in VX is NOT pressed
        case 0x00A1:
        if (key[V[(opcode & 0x0F00) >> 8]] == 0)
        PC+=4;
        else
        PC+=2;
        break;

        default:
        printf("\nUnknown opcode: %.4X\n", opcode);
        exit(3);
    }
    break;

    // FX: Timers
    case 0xF000:
        switch(opcode & 0x00FF)
    {       
            // FX07: sets VX to the current value of the delay timer
            case 0x0007:
            V[(opcode & 0x0F00) >> 8] = d_timer;
            PC+=2;
            break;

            // FX0A: Get key - A key press is awaited, and then stored in VX 
            case 0x000A:
            {
                bool key_press = false;

                for(int i =0; i < 16; ++i)
                    {
                        if(key[i] != 0)
                        {
                            V[(opcode & 0x0F00) >> 8 ] = i;
                            key_press = true;
                        }
                    }
                if(!key_press)
                    return;

                PC+=2;
            }
        break;

        // FX15 - sets the delay timer to the value in VX
        case 0x0015:
        d_timer = V[(opcode & 0x0F00) >> 8];
        PC+=2;
        break;

        // FX18 - sets the sound timer to the value in VX
        case 0x0018:
        s_timer = V[(opcode & 0x0F00) >> 8];
        PC+=2;
        break;

        // FX1E - Add to index, index register I will get the value in VX added to it.
        // VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
        case 0x001E:
            if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
            V[0xF] = 1;
            else
            V[0xF] = 0;

            I += V[(opcode & 0x0F00) >> 8];
            PC+=2;
        break;

        // FX29 - Font character - Set I to the location for the character sprite in VX.
        case 0x0029:
        I = V[(opcode & 0x0F00) >> 8] * 0x5;
        PC+=2;
        break;

        /**
         * FX33: Binary-coded decimal conversion - 
         * It takes the number in VX (which is one byte, so it can be any number from 0 to 255) 
         * and converts it to three decimal digits, 
         * storing these digits in memory at the address in the index register I. 
         */
        case 0x0033:
            MEM[(I+0) & 0xFFF] = V[(opcode & 0x0F00) >> 8] / 100;
            MEM[(I + 1) & 0xFFF] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            MEM[(I + 2) & 0xFFF] = V[(opcode & 0x0F00) >> 8] % 10;
            PC+=2;
        break;

        // FX55: - Store and load Memory
        /**
         * the value of each variable register from V0 to VX inclusive (if X is 0, then only V0) 
         * will be stored in successive memory addresses, 
         * starting with the one that’s stored in I. V0 will be stored at the address in I, 
         * V1 will be stored in I + 1, and so on, until VX is stored in I + X.
         * 
         */
        case 0x0055:
        for(int i = 0; i <= ((opcode & 0x0F00)>> 8 ); ++i)
        {
            MEM[(I+i) & 0xFFF] = V[i];
        }
            I += ((opcode & 0x0F00) >> 8) + 1;
            PC+=2;
            break;
        
        /**
         * FX65 does the same thing, except that it takes the value stored at the memory addresses 
         * and loads them into the variable registers instead.
         */
        case 0x0065:
        for(int i = 0; i <= ((opcode & 0x0F00)>> 8 ); ++i)
        {
            V[i]=MEM[(I+i) & 0xFFF];
        }
            I += ((opcode & 0x0F00) >> 8) + 1;
            PC+=2;
            break;
        default:
        printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
    }

    break;

    default:
    printf("\nUnimplemented op code: %.4X\n", opcode);
    exit(3);
    }

/**
 * Timer updates
 * Along with the simple implementation of sound 
 */
if(d_timer > 0)
    {
    d_timer--;
    }

   if(s_timer > 0)
    if(s_timer == 1)
    {
        playsound = true;
    }

   --s_timer;
}  