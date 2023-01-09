#ifndef CHIP8
#define CHIP8

#include <stdio.h>
#include <stdint.h>

class chip_8
{
private:

    uint8_t V[16];
    uint8_t s_timer;
    uint8_t d_timer;

    uint8_t MEM[4096];

    uint16_t I;
    uint16_t PC;
    uint16_t stack[16];
    uint16_t SP;

    uint16_t opcode;

    void init();


public:

uint8_t display[64*32];
uint8_t key[16];

bool drawflag;
bool playsound = false;

void emu_cycle();

bool load(const char *file_path);

chip_8();           // constructor
~chip_8();          // deconstructor

};

#endif