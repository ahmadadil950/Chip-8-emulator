#include <iostream>
#include <thread>
#include <chrono>
#include "stdint.h"
#include <SDL.h> 
#include "chip8.h"
using namespace std;


uint8_t keymap[16] = 
{
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

void playsound(SDL_AudioDeviceID device_ID, uint8_t *waveBuffer, uint32_t waveLength)
{
    int play = SDL_QueueAudio(device_ID,waveBuffer,waveLength);
    SDL_PauseAudioDevice(device_ID,0);
}

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cout<<"Usage: Chip8 [ROM File]" <<endl;
        return 1;
    }

    chip_8 Chip8 = chip_8();
    
        int w = 1024;
        int h = 512;

    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not be initalized SDL ERROR: %s\n",SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow
    (
        "CHIP-8 EMULATOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,w, h, SDL_WINDOW_SHOWN
    );

    if(window==NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n",SDL_GetError());
        exit(2);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,0);
    SDL_RenderSetLogicalSize(renderer,w,h);

    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,64, 32);

    /*Loading in audio*/
    SDL_AudioSpec wav_Spec;
    uint32_t wavlength;
    uint8_t* wave_Buffer;

    SDL_LoadWAV("music.wav", &wav_Spec, &wave_Buffer, &wavlength);

    SDL_AudioDeviceID device_ID = SDL_OpenAudioDevice(NULL,0,&wav_Spec,NULL,0);

    uint32_t pixel[2048];

    load:

    if(!Chip8.load(argv[1]))
    return 2;

    while(true)
    {
        Chip8.emu_cycle();


        SDL_Event A;

        while(SDL_PollEvent(&A))
        {
            if(A.type == SDL_KEYDOWN)
            {
                if(A.key.keysym.sym == SDLK_ESCAPE)
                exit(0);
            }

            if(A.key.keysym.sym == SDLK_F1)
            goto load;

            for(int i =0; i< 16; ++i)
            {
                if(A.key.keysym.sym == keymap[i])
                {
                    Chip8.key[i] = 1;
                }
            }
        }

        if(A.type == SDL_KEYUP)
        {
            for(int i =0; i<16; ++i)
            {
                if(A.key.keysym.sym == keymap[i])
                {
                    Chip8.key[i] = 0;
                }
            }
        }


    if(Chip8.drawflag)
    {
        Chip8.drawflag = false;


        for(int i =0; i<2048; ++i)
        {
            uint8_t P = Chip8.display[i];
            pixel[i] = (0x00FFFFFF * P) | 0xFF000000;
        }

        SDL_UpdateTexture(sdlTexture, NULL, pixel, 64 * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
    }

    if(Chip8.playsound)
    {
        playsound(device_ID,wave_Buffer,wavlength);
        Chip8.playsound = false;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1800));
    }
}