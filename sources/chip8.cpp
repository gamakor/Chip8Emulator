//
// Created by gamakor on 4/20/25.
//

#include "chip8.h"
#include "raylib.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <linux/input-event-codes.h>




void chip8::Initialize() {
    pc= 0x200;
    opcode= 0;
    I= 0;
    sp = 0;

    //clear display
    for (int i = 0; i < (64*32);++i) {
        gfx[i] = 0;
    }
    //clear stack
    for (int i = 0; i < 16;++i) {
        stack[i] = 0;
    }
    for (int i = 0; i < 16;++i) {
        V[i] = 0;
    }
    for (int i = 0; i < 4096;++i) {
        memory[i] = 0;
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
                case 0x0000: // clears the screen
                    //Clear the arrary for gfx
                    for (auto& element : gfx) {
                        element = 0;
                    }
                    drawFlag = true;
                    printf("Clear screen\n");
                    pc +=2;
                    break;
                case 0x000E: // returns from a subroutines

                    --sp;
                    pc = stack[sp];
                    pc+=2;
                    break;
                default: ;
            }
            break;
        case 0x1000: //Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;


            break;
        case 0x3000://skip next opcode if vX == NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;


        case 0x4000://skip next opcode if vX != NN
            if (V[(opcode & 0x0f00) >>8]!= (opcode & 0x00ff))
                pc += 4;
            else
                pc += 2;
                    break;
        case 0x5000://skip next opcode if vX == vY
            if (V[(opcode & 0x0f00) >>8] == V[(opcode & 0x00f0)>>4])
                pc += 4;
            else
                pc += 2;
                    break;

        case 0x6000://set vX to NN
            V[(opcode & 0x0f00)>> 8] = opcode &0x00ff;
            pc+=2;
            break;
        case 0x7000: // add nn to VX
            V[(opcode & 0x0f00)>>8] += opcode & 0x00ff;
            pc +=2;
            break;
        case 0x8000:
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;

            switch (opcode & 0x000F) {
                case 0x0000: // 8XY0 - Set VX = VY
                    V[x] = V[y];
                    pc += 2;
                    break;

                case 0x0001: // 8XY1 - Set VX = VX OR VY
                   V[x] |= V[y];
                    pc += 2;
                    break;

                case 0x0002: // 8XY2 - Set VX = VX AND VY
                    V[x] &= V[y];
                    pc += 2;
                    V[0xF] = 0;
                    break;

                case 0x0003: // 8XY3 - Set VX = VX XOR VY
                    V[x] ^= V[y];
                    pc += 2;
                    break;

                case 0x0004: // 8XY4 - Add VY to VX, set VF = carry
                {
                    uint16_t sum = V[x] + V[y];
                    V[x] = sum & 0xFF;
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    pc += 2;
                }
                    break;

                case 0x0005: {
                    // 8XY5 - Subtract VY from VX, set VF = NOT borrow
                    char VFlag = (V[x] >= V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    V[0xF] = VFlag;
                    pc += 2;
                    break;
                }
                case 0x0006: {
                    // 8XY6 - Shift VX right by 1, store LSB in VF
                    char vFlag = V[x] & 0x1;
                    V[x] >>= 1;
                    V[0xF] = vFlag;
                    pc += 2;
                    break;
                }

                case 0x0007: {
                    // 8XY7 - Set VX = VY - VX, set VF = NOT borrow
                    char vFlag = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    V[0xF] = vFlag;
                    pc += 2;
                    break;
                }
                case 0x000E: {
                    // 8XYE - Shift VX left by 1, store MSB in VF
                    char vFlag = (V[x] & 0x80) >> 7;
                    V[x] <<= 1;
                    V[0xF] = vFlag;
                    pc += 2;
                    break;
                }
                default:
                    std::cerr << "Unknown opcode [0x8000 group]: 0x" << std::hex << opcode << std::endl;
                    break;
            }
        }
            break;

        case 0x9000: //Skip next opcode if vX != vy
            if (V[(opcode & 0x0f00)>>8]!=V[(opcode & 0x00f0)>>4])
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
            pc = V[0] + (opcode & 0x0FFF);
            break;
        case 0xC000:
            V[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode&0x00FF);
            pc += 2;
            break;

        case 0xD000: {
            const unsigned short x = V[(opcode & 0x0F00) >> 8];
            const unsigned short y = V[(opcode & 0x00F0) >> 4];
            const unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;

            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        int x_coord = (x + xline) % 64;  // wrap horizontally
                        int y_coord = (y + yline) % 32;  // wrap vertically
                        int index = x_coord + (y_coord * 64);

                        if (index >= 0 && index < 2048) { // bounds check
                            if (gfx[index] == 1) {
                                V[0xF] = 1;
                            }
                            gfx[index] ^= 1;
                        }
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
        case 0xE000:
            switch (opcode & 0x00ff) {
                case 0x009E:// Skips the next instruct if the key stored in VX is pressed
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
                        pc += 4;

                    }  else {
                        pc +=2;

                    }

                    break;
                case 0x00A1: // Skips the next instruction if the key stored in VX is not pressed
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
                    V[(opcode & 0x0F00) >> 8 ] = delay_timer;
                    pc += 2;
                    break;
            case 0x000A: {
                if (!waitingForKey) {
                    waitingForKey = true;
                    waitingRegister = (opcode & 0x0F00) >> 8;
                    return; // wait until a key is pressed
                }

                for (int i = 0; i < 16; ++i) {
                    if (key[i] != 0) {
                        V[waitingRegister] = i;
                        waitingForKey = false;
                        pc += 2;
                        break;
                    }
                }
                break;
            }
                case 0x0015: // sets the delay timer to vx
                    delay_timer = V[(opcode & 0x0F00) >> 8 ];
                    pc += 2;
                    break;
                case 0x0018:// set the sound timer to VX
                    sound_timer = V[(opcode & 0x0F00)>>8];
                    pc +=2;
                    break;
                case 0x001E:// adds VX to I. VF is not affected
                    I += V[(opcode & 0x0F00)>>8];
                    pc += 2;
                    break;
                case 0x0029: // sets I to the mem location of the sprite for the character in vx
                    I = V[(opcode & 0x0F00)>>8] *5;
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
                    for (int i = 0; i <= ((opcode & 0x0F00)>>8); ++i) {
                        memory[I+i] = V[i];
                    }
                    pc += 2;
                    break;
                case 0x0065: // fills from v0 to vx (including vx ) with values from mem starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >>8); ++i) {
                        V[i] = memory[I + i];
                    }
                    //fI += ((opcode & 0x0F00) >>8) +1;
                    pc += 2;
                    break;
                default:
                    break;
            }
            break;

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
    FILE* f = fopen(ASSETS_PATH"glitchGhost.ch8", "rb");

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

  /*1   2 	3 	C
    4 	5 	6 	D
    7 	8 	9 	E
    A 	0 	B 	F */
}

void chip8::SetKeys() {

    memset(key, 0, sizeof(key));  // Clear all keys first

   // key[0x6] = 1;
    if (IsKeyDown(KEY_ONE))
    {
        std::cout << "key 1" << std::endl;
        key[0x1] = 1;
    }
    else if (IsKeyUp(KEY_ONE))
    {
        key[0x1] = 0;
    }
    if (IsKeyDown(KEY_TWO))
    {
        std::cout << "key 2" << std::endl;
        key[0x2] = 1;
    }
    else if (IsKeyUp(KEY_TWO))
    {
        key[0x2] = 0;
    }
    if (IsKeyDown(KEY_THREE))
    {
        std::cout << "key 3" << std::endl;
        key[0x3] = 1;
    }
    else if (IsKeyUp(KEY_THREE))
    {
        key[0x3] = 0;
    }
    if (IsKeyDown(KEY_FOUR))
    {
        std::cout << "key 4" << std::endl;
        key[0xC] = 1;
    }
    else if (IsKeyUp(KEY_FOUR))
    {
        key[0xC] = 0;
    }
    if (IsKeyDown(81))//q
    {
        std::cout << "key q" << std::endl;
        key[0x4] = 1;
    }
    else if (IsKeyUp(81))
    {
        key[0x4] = 0;
    }
    if (IsKeyDown(87))
    {
        std::cout << "key w" << std::endl;
        key[0x5] = 1;
    }
    else if (IsKeyUp(87))
    {
        key[0x5] = 0;
    }
    if (IsKeyDown(69))
    {
        std::cout << "key e" << std::endl;
        key[0x6] = 1;
    }
    else if (IsKeyUp(69))
    {
        key[0x6] = 0;
    }
    if (IsKeyDown(82))
    {
        std::cout << "key r" << std::endl;
        key[0xD] = 1;
    }
    else if (IsKeyUp(82))
    {
        key[0xD] = 0;
    }
    if (IsKeyDown(65))
    {
        std::cout << "key a" << std::endl;
        key[0x7] = 1;
    }
    else if (IsKeyUp(65))
    {
        key[0x7] = 0;
    }
    if (IsKeyDown(83))
    {
        std::cout << "key s" << std::endl;
        key[0x8] = 1;
    }
    else if (IsKeyUp(83))
    {
        key[0x8] = 0;
    }
    if (IsKeyDown(68))
    {
        std::cout << "key d" << std::endl;
        key[0x9] = 1;
    }
    else if (IsKeyUp(68))
    {
        key[0x9] = 0;
    }
    if (IsKeyDown(70))
    {
        std::cout << "key f" << std::endl;
        key[0xE] = 1;
    }
    else if (IsKeyUp(70))
    {
        key[0xE] = 0;
    }
    if (IsKeyDown(90))
    {
        std::cout << "key z" << std::endl;
        key[0xA] = 1;
    }
    else if (IsKeyUp(90))
    {
        key[0xA] = 0;
    }
    if (IsKeyDown(88))
    {
        std::cout << "key x" << std::endl;
        key[0x0] = 1;
    }
    else if (IsKeyUp(88))
    {
        key[0x0] = 0;
    }
    if (IsKeyDown(67))
    {
        std::cout << "key c" << std::endl;
        key[0xB] = 1;
    }
    else if (IsKeyUp(67))
    {
        key[0xB] = 0;
    }
    if (IsKeyDown(86))
    {
        std::cout << "key v" << std::endl;
        key[0xF] = 1;
    }
    else if (IsKeyUp(86))
    {
        key[0xF] = 0;
    }
}
