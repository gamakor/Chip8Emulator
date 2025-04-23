//
// Created by gamakor on 4/20/25.
//

#ifndef CHIP8_H
#define CHIP8_H



class chip8 {
public:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;

    unsigned char gfx[64*32];

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16];
    unsigned short sp;

    unsigned char key[16];

void Initialize();
    void EmulateCycle();

};



#endif //CHIP8_H
