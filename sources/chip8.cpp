//
// Created by gamakor on 4/20/25.
//

#include "chip8.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

void chip8::Initialize() {
    pc= 0x200;
    opcode= 0;
    I= 0;
    sp = 0;

    //clear display
    for (auto element: gfx) {
        gfx[element] = 0;
    }
    //clear stack
    for (auto element: stack) {
        stack[element] = 0;
    }
    //clear registers v0-vf
    for (auto element: V) {
        V[element] = 0;
    }
    //clear memory
    for (auto element: memory) {
        memory[element] = 0;
    }

    delay_timer = 0;
    sound_timer = 0;

    //load fontset
    for (int i = 0; i < 80; i++) {
        memory[i] = chip8_fontset[i];
    }




}

void chip8::EmulateCycle() {
    //fetch opcode
    opcode = memory[pc]<<8 | memory[pc+1];

    //decode opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // clears the screen todo
                    //Clear the arrary for gfx
                    for (auto element: gfx) {
                        gfx[element] = 0;
                    }
                    drawFlag = true;
                    printf("Clear screen\n");
                    pc +=2;
                    //set draw flag
                    break;
                case 0x000E: // returns from a subroutines todo
                    pc = stack[sp];
                    sp--;
                    break;
            }
            break;
        case 0x1000: //Jump to address NNN
            pc = opcode & 0x0FFF;
            printf("Jump to address\n");
            break;
        break;
        case 0x2000: // Call subroutine at NNN

            sp++;
            stack[sp] = pc;
            pc = opcode & 0x0FFF;
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
            printf("Set vx to NN""\n");
            break;
        case 0x7000: // add nn to VX
            V[opcode & 0x0f00] += opcode & 0x00ff;
            pc +=2;
            printf("Add nn to VX \n");
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
            break;
        case 0x9000: //Skip next opcode if vX != vy
            if (V[opcode & 0x0f00]==V[opcode & 0x00f0])
                pc+= 4;
            else
                pc+= 2;

            break;
        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            printf("ANNN\n");
            break;
        case 0xB000:// bnnn:
            pc = V[0] + opcode & 0x0FFF;
            pc+= 2;
            break;
        case 0xC000:
            V[opcode & 0x0F00] = (rand() % 255) & 0x00FF;
            pc += 2;
            break;

        case 0xD000: {
            const unsigned short x = V[(opcode & 0x0F00)>>8];
            const unsigned short y = V[(opcode & 0x00F0)>>4];
            const unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xf] = 0;

            for (int yline = 0 ; yline < height ; yline++) {
                pixel = memory[I+yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel &(0x80 >> xline))!= 0){
                        V[0xF] = 1;
                    }
                    if (gfx[(x+xline+ (y + yline)* 64)] == 1) {
                        gfx[x + xline + (y+yline) * 64] ^= 1;
                    }

                }

            }
            drawFlag = true;
            pc +=2;
            printf("Draww\n");
            break;
        }
        case 0xE000:
            switch (opcode & 0x000f) {
                case 0x000E:// Skips the next instruct if the key stored in VX is pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
                        pc += 4;
                    }  else {
                            pc +=2;
                        }

                    break;
                case 0x0001: // Skips the next instruction if the key stored in VX is not pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
                        pc += 4;
                    }  else {
                        pc +=2;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: // sets VX to the value of the delay timer
                    V[opcode & 0x0F00] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A: // A key press is awaited, and then stored in VX
                    for (unsigned char key1: key) {
                        if (key1 == 1) {
                            V[opcode & 0x0F00] = key[key1];
                            pc += 2;
                        }
                    }
                    break;
                case 0x0015: // sets the delay timer to vx
                    delay_timer = V[opcode & 0x0F00];
                    pc += 2;
                    break;
                case 0x0018:// set the sound timer to VX
                    sound_timer = V[opcode & 0x0F00];
                    pc +=2;
                    break;
                case 0x001E:// adds VX to I. VF is not affected
                    I += V[opcode & 0x0F00];
                    pc += 2;
                    break;
                case 0x0029: // sets I to the mem location of the sprite for the character in vx
                    I = V[opcode & 0x0F00];
                    pc += 2;
                    break;
                case 0x0033: // stores the binary - coded decimal represenation of vx , with the hundresds digit in memory at location I,
                    memory[I] = V[(opcode & 0x0F00)>>8] / 100;
                    memory[I+1] = (V[(opcode & 0x0F00)>>8] / 10) % 10;
                    memory[I+2] = (V[(opcode & 0x0F00)>>8] % 100) % 10;
                    pc+= 2;
                    break;
                case 0x0055: // stores from v0 to vx in mem, starting at address I The offset from I is increased by 1 for each value written but I itself is left unmodifeid
                    // for loop from v0 to vx
                    for (int i = 0; i < (opcode & 0x0F00); ++i) {
                        memory[I+i] = V[i];
                    }
                    pc += 2;
                    break;
                case 0x0065: // fills from v0 to vx (including vx ) with values from mem starting at address I
                    for (int i = 0; i < (opcode & 0x0F00); ++i) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;
                default:
                    break;
            }

            default:
            printf("unknown opcode 0x%04X\n", opcode);
            break;
    }

    if (delay_timer >0) {
        --delay_timer;
    }
    if (sound_timer >0) {
        printf("Beep!\n");
        --sound_timer;
    }


}

void chip8::LoadGame() {
    // fopen()
    FILE* f = fopen(ASSETS_PATH"2-ibm-logo.ch8", "rb");

    if (f == nullptr) {
        printf("Error opening file\n");
    } else {
        printf("Opening file\n");
    }

    fseek(f, 0, SEEK_END);
    long rom_size = ftell(f);
    rewind(f);

    char* rom_buffer = static_cast<char *>(malloc(sizeof(char) * rom_size));
    if (rom_buffer == nullptr) {
        std::cerr << "Failed to allocate memory for ROM" << std::endl;
    }

    // Copy ROM into buffer
    size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, f);
    if (result != rom_size) {
        std::cerr << "Failed to read ROM" << std::endl;

    }


    // Copy buffer to memory
    if ((4096-512) > rom_size){
        for (int i = 0; i < rom_size; ++i) {
            memory[i + 512] = (uint8_t)rom_buffer[i];   // Load into memory starting
            // at 0x200 (=512)
        }
    }
    else {
        std::cerr << "ROM too large to fit in memory" << std::endl;

    }


    fclose(f);
    free(rom_buffer);


}
