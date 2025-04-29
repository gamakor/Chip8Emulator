
#include <cstdio>

#include "raylib.h"
#include "chip8.h"

#define SCREEN_WIDTH (640)
#define SCREEN_HEIGHT (320)

#define WINDOW_TITLE "Window title"

chip8 myChip8;

void DrawGraphics() {
    myChip8.drawFlag =false;
    ClearBackground(BLACK);
    BeginDrawing();

   int pixelOn = 0;

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (myChip8.gfx[x + (64 * y)]) {
                DrawRectangle(x * 10, y * 10, 10, 10, WHITE);
                pixelOn++;
            }
        }

    }
    EndDrawing();
   printf("%i",pixelOn);
}


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Texture2D texture = LoadTexture(ASSETS_PATH"test.png"); // Check README.md for how this works

    //SetupGraphics();
    //SetupInput();

    myChip8.Initialize();
    myChip8.LoadGame();
    int cycles = 0;

    while (!WindowShouldClose())
    {

            //impulate one cycle
            myChip8.EmulateCycle();
            //if draw flag update screen
            if(myChip8.drawFlag) {
                DrawGraphics();
            }
            cycles++;

        //MyChip8.setKeys();
    }

    CloseWindow();

    return 0;
}
