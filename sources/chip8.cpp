//
// Created by gamakor on 4/20/25.
//

#include "chip8.h"

#include <cstdio>
#include <cstdlib>

void chip8::Initialize() {
    pc= 0x200;
    opcode= 0;
    I= 0;
    sp = 0;

    //clear display
    for (auto element: gfx) {
        gfx[element] = '\n';
    }
    //clear stack
    for (auto element: stack) {
        stack[element] = '\n';
    }
    //clear registers v0-vf
    for (auto element: V) {
        V[element] = '\n';
    }
    //clear memory
    for (auto element: memory) {
        memory[element] = '\n';
    }

    delay_timer = '\n';
    sound_timer = '\n';

    //load fontset
    for (int i = 0; i < 80; i++) {
       // fontset
    }




}

void chip8::EmulateCycle() {
    //fetch opcode
    opcode = memory[pc]<<8 | memory[pc+1];

    //decode opcode
    switch (opcode & 0xF000) {

        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        break;
        case 0x2000:
            break;
        case 0x3000://skip next opcode if vX == NN
            if (V[opcode & 0x0f00]== opcode & 0x0ff)
                pc+=4;
            else
                pc+=2;
            break;
        case 0x4000://skip next opcode if vX != NN
            if (V[opcode & 0x0f00]!= opcode & 0x0ff)
                pc += 4;
            else
                pc += 2;
                    break;
        case 0x5000://skip next opcode if vX == vY
            if (V[opcode & 0x0f00] == V[opcode & 0x00f0])
                pc += 4;
            else
                pc += 2;
                    break;

        case 0x6000://set vX to NN
            V[opcode & 0x0f00] = opcode &0x00ff;
            pc+=2;
            break;
        case 0x7000:
            V[opcode & 0x0f00] += opcode & 0x00ff;
            pc +=2;
            break;
        case 0x8000:
            switch (opcode & 0x000f) {
            case 0x0000://set vX to NN
                    V[opcode & 0x0f00]= V[opcode & 0x00f0];
                    pc+=2;
                    break;
            case 0x0001://Set vsx to vx or vy(bitwise or operator
                    V[opcode & 0x0f00]=  V[opcode & 0x0f00] |V[opcode & 0x00f0];
                    pc+=2;
                    break;
            case 0x0002:    //Sets VX to VX and VY (bitwise and
                    V[opcode & 0x0f00] &= V[opcode & 0x00f0];
                    pc+=2;
                    break;
            case 0x0003:    //Sets VX to VX xor Vy
                    V[opcode & 0x0f00]^= V[opcode & 0x00f0];
                    pc+=2;
                    break;
            case 0x0004:    //Adds VY to VX VF is set to 1 when theres's an overflow ,0 when not
                    V[opcode & 0x0f00]+= V[opcode & 0x00f0];
                    // set vf to true if there is over flow and to 0 if there isnt
                    //TOdo how to test for over flow with out over flowing?
                    //try taking vx figuring out if dif to 255 is smalling than vf if there is flag?
                    pc+=2;
                    break;
            case 0x0005:
                    //check for underflow
                    //if underflow 0 if not 1 on vf
                    V[opcode & 0x0f00]-= V[opcode & 0x00f0];
                    pc += 2;
                    break;
            case 0x0006:    //Shifst VX to the right by 1
                    //right shift bit op need to research
                    break;
            case 0x0007:
                    //set vf flag 0 for underflow
                    V[opcode & 0x0f00]= V[opcode & 0x00f0] - V[opcode & 0x0f00] ;
                    pc+=2;
                    break;
            case 0x000E:
                    //left shift bit op
                    break;



            }
        case 0x9000: //Skip next opcode if vX != vy
            if (V[opcode & 0x0f00]==V[opcode & 0x00f0])
                pc+= 4;
            else
                pc+= 2;
        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:// bnnn:
            pc = V[0] + opcode & 0x0FFF;
            pc+= 2;
            break;
        case 0xC000:
            V[opcode & 0x0F00] = (rand() % 255) & 0x00FF;
            pc += 2;
            break;

        case 0xD00:
            //Draw opcode need to figure out if i need to have raylib calls here or just chip 8

            default:
            printf("unknown opcode 0x%04X\n", opcode);
    }

    if (delay_timer >0) {
        --delay_timer;
    }
    if (sound_timer >0) {
        printf("Beep!\n");
        --sound_timer;
    }


}
