#include "raylib.h"
#include "game.h"
#include "input.h"
#include "render.h"

#include <stdio.h>

int main(void)
{
    // Ensure data directory exists for highscore file
    MakeDirectory("data");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Block Blast");
    SetTargetFPS(60);

    GameState state;
    GameInit(&state);

    while (!WindowShouldClose()) {
        // Input (only during play screen)
        if (state.currentScreen == SCREEN_PLAY) {
            InputUpdate(&state);
        }

        // Update
        GameUpdate(&state);

        // Draw
        RenderFrame(&state);
    }

    // Cleanup: free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state.slots[i].piece);

    CloseWindow();
    return 0;
}