
#include <cstdio>
#include <thread>

#include "raylib.h"
#include "chip8.h"

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (320)

#define WINDOW_TITLE "CHIP 8 EMU"

chip8 myChip8;



void DrawGraphics() {
    myChip8.drawFlag =false;


   // myChip8.SetKeys();
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (myChip8.gfx[x + (64 * y)]) {
                DrawRectangle(x * 10, y * 10, 10, 10, WHITE);

            }
        }
    }

}




int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    InitAudioDevice();
    //SetTargetFPS(360);

    Texture2D texture = LoadTexture(ASSETS_PATH"test.png"); // Check README.md for how this works

    //SetupGraphics();
    //SetupInput();

    myChip8.Initialize();
    myChip8.LoadGame();

    while (!WindowShouldClose())
    {
        myChip8.SetKeys();
        ClearBackground(BLACK);
        BeginDrawing();

        //impulate one cycle
        myChip8.EmulateCycle();
            //if draw flag update screen
        myChip8.UpdateTimers();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        DrawGraphics();
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
