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
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // clears the screen
                    break;
                case 0x000E: // returns from a subroutines
                    break;
            }
        case 0x1000: //Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        break;
        case 0x2000: // Call subroutine at NNN
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
        case 0x7000: // add nn to VX
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
                    if (V[(opcode & 0x00F0)>> 4] > (0xFF - V[opcode & 0x0F00]>> 8) )
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[opcode & 0x0f00 >> 8]+= V[opcode & 0x00f0 >> 4];
                    pc+=2;
                    break;
            case 0x0005: // subtract VY from VX set flag to 0  for underflow
                    //check for underflow
                    //if underflow 0 if not 1 on vf
                    if (V[(opcode & 0x00F0)>> 4] < (0xFF - V[opcode & 0x0F00]>> 8) )
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[opcode & 0x0f00]-= V[opcode & 0x00f0];
                    pc += 2;
                    break;
            case 0x0006:    //Shifst VX to the right by 1
                    //right shift bit op need to research
                    V[0xF] = opcode & 0x000f;
                    V[opcode & 0x0f00] =  V[opcode & 0x0f00]>>1;
                    break;
            case 0x0007:
                    //set vf flag 0 for underflow
                    if (V[(opcode & 0x0F00)>> 8] < (0xFF - V[opcode & 0x00F0]>> 4) )
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[opcode & 0x0f00]= V[opcode & 0x00f0] - V[opcode & 0x0f00] ;
                    pc+=2;
                    break;
            case 0x000E:
                    //left shift bit op
                    V[0xF] = opcode & 0x000f;
                    V[opcode & 0x0f00] =  V[opcode & 0x0f00]<<1;
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

        case 0xD000:
            //Draw opcode need to figure out if i need to have raylib calls here or just chip 8
            break;
        case 0xE000:
            switch (opcode & 0x000f) {
                case 0x000E:// Skips the next instruct if the key stored in VX is pressed
                    break;
                case 0x0001: // Skips the next instruction if the key stored in VX is not pressed
                    break;

            }
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // sets VX to the value of the delay timer
                    break;
                case 0x000A: // A key press is awaited, and then stored in VX
                    break;
                case 0x0015: // sets the delay timer to vx
                    break;
                case 0x0018:// set the sound timer to VX
                    break;
                case 0x001E:// adds VX to I. VF is not affected
                    break;
                case 0x0029: // sets I to the location of the sprite for the character in vx
                    break;
                case 0x0033: // stores the binary - coded decimal represenation of vx , with the hundresds digit in memory at location I,
                    memory[I] = V[(opcode & 0x0F00)>>8] / 100;
                    memory[I+1] = (V[(opcode & 0x0F00)>>8] / 10) % 10;
                    memory[I+2] = (V[(opcode & 0x0F00)>>8] % 100) % 10;
                    pc+= 2;
                    break;
                case 0x0055: // stores from v0 to vx in mem, starting at address I The offset from I is increased by 1 for each value written but I itself is left unmodifeid
                    break;
                case 0x0065: // fills from v0 to vx (including vx ) with values from mem starting at address I
                    break;

            }

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
