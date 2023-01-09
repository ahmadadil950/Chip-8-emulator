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

void chip_8::init()
{
    for(int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        key[i] = 0;
        V[i] = 0;
    }


    PC = 0x200;
    opcode = 0;
    SP = 0;
    I = 0;

    for(int i = 0; i < 2048; ++i)
    {
        display[i] = 0;
    }

    for(int i = 0; i < 4096; ++i)
    {
        MEM[i] = 0;
    }

    for(int i = 0; i < 80; ++i)
    {
        MEM[i] = fontset[i];
    }

    s_timer = 0;
    d_timer = 0;

    
    srand(time(NULL));

}


bool chip_8::load(const char *file_path)
{
    init();

    printf("Loading ROM: %s\n", file_path);

    FILE* rom = fopen(file_path, "rb");

    if(rom == NULL)
    {
        cout << "failed to open" << endl;
        return false;
    }

    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    char* rom_buffer = (char*)malloc(sizeof(char)*rom_size);
    if(rom_buffer == NULL)
    {
        cout<<"failed to allocate memory for ROM" << endl;
        return false;
    }

    size_t result = fread(rom_buffer,sizeof(char),(size_t)rom_size,rom);
    if(result != rom_size)
    {
        cout<<"failed to to read rom"<<endl;
        return false;
    }

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

    fclose(rom);
    free(rom_buffer);

    return true;
}


void chip_8::emu_cycle()
{
    opcode = MEM[PC] << 8 | MEM[PC+1];

    switch (opcode & 0xF000)
    {
        case 0x0000:

        switch(opcode & 0x000F)
        {
        case 0x0000:
        for(int i = 0; i<2048; ++i)
        {
            display[i] = 0;
        }

        drawflag = true;

        PC += 2;
        
        break;

    
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


case 0x1000:
PC = opcode & 0x0FFF;
break;

case 0x2000:
stack[SP] = PC;
++SP;
PC = opcode & 0x0FFF;
break;

case 0x3000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

case 0x4000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

case 0x5000:
if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
{
    PC += 4; 
}
else{
    PC+=2;
}
break;

case 0x6000:
V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
PC += 2;
break;

case 0x7000:
V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
PC += 2;
break;


case 0x8000:
    switch (opcode & 0x000F)
    {
        case 0x0000:
        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00FF) >> 4]; // 8XY0 - Sets VX to the value of VY.
        PC+=2;
        break;


        case 0x0001:
        V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        case 0x0002:
        V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

        case 0x0003:
        V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00FF) >> 4];
        PC+=2;
        break;

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


        case 0x0006:
        V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
        V[(opcode & 0x0F00) >> 8] >>=1;
        PC+=2;
        break;


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


    case 0x9000:
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
    PC += 4;
    else
    PC += 2;
    break;


    case 0xA000:
    I = opcode & 0x0FFF;
    PC+=2;
    break;


    case 0xB000:
    PC = (opcode & 0x0FFF) + V[0];
    break;

    case 0xC000:
    V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF+1)) & (opcode & 0x00FF);
    PC += 2;
    break;


    case 0xD000:
    {
        unsigned short X = V[(opcode & 0x0F00) >> 8];
        unsigned short Y = V[(opcode & 0x00F0) >> 4];
        unsigned short h = opcode & 0x000F;
        unsigned short p;

        V[0xF] = 0;

        for(int yLine = 0; yLine < h; yLine++)
        {
            p = MEM[I + yLine];
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

    case 0xE000:
    switch(opcode & 0x00FF)
    {
        case 0x009E:
        if (key[V[(opcode & 0x0F00) >> 8]] != 0)
        PC+=4;
        else
        PC+=2;
        break;

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


    case 0xF000:
        switch(opcode & 0x00FF)
    {
            case 0x0007:
            V[(opcode & 0x0F00) >> 8] = d_timer;
            PC+=2;
            break;

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

        case 0x0015:
        d_timer = V[(opcode & 0x0F00) >> 8];
        PC+=2;
        break;

        case 0x0018:
        s_timer = V[(opcode & 0x0F00) >> 8];
        PC+=2;
        break;

        case 0x001E:
            if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
            V[0xF] = 1;
            else
            V[0xF] = 0;

            I += V[(opcode & 0x0F00) >> 8];
            PC+=2;
        break;

        case 0x0029:
        I = V[(opcode & 0x0F00) >> 8] * 0x5;
        PC+=2;
        break;

        case 0x0033:
        MEM[I] = V[(opcode & 0x0F00) >> 8] / 100;
        MEM[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
        MEM[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
        PC+=2;
        break;

        case 0x0055:
        for(int i = 0; i <= ((opcode & 0x0F00)>> 8 ); ++i)
        {
            MEM[I+i] = V[i];
        }
            I += ((opcode & 0x0F00) >> 8) + 1;
            PC+=2;
            break;
        
        case 0x0065:
        for(int i = 0; i <= ((opcode & 0x0F00)>> 8 ); ++i)
        {
            V[i]=MEM[I+i];
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


if(d_timer > 0)
    {
    d_timer--;
    }

   if(s_timer > 0)
   if(s_timer == 1)
   {
       playsound = true;
   }

   s_timer--;
}  