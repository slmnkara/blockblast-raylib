#include "game.h"
#include "score.h"
#include <string.h>

void GameInit(GameState *state)
{
    memset(state, 0, sizeof(GameState));
    state->currentScreen = SCREEN_MENU;
    BoardInit(&state->board);
    state->highScore = ScoreLoadHigh();
    state->combo = 0;
    state->score = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;

    // Slots start empty until play begins
    for (int i = 0; i < 3; i++)
        state->slots[i].piece = NULL;
}

void GameReset(GameState *state)
{
    // Free any remaining pieces
    for (int i = 0; i < 3; i++)
        PieceFree(&state->slots[i].piece);

    BoardInit(&state->board);
    state->score = 0;
    state->combo = 0;
    state->isDragging = false;
    state->gameOver = false;
    state->anims.count = 0;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
}

void GameUpdate(GameState *state)
{
    float dt = GetFrameTime();

    switch (state->currentScreen) {
        case SCREEN_MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;

        case SCREEN_PLAY:
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Check game over transition
            if (state->gameOver) {
                state->currentScreen = SCREEN_GAMEOVER;
            }
            break;

        case SCREEN_GAMEOVER:
            if (IsKeyPressed(KEY_ENTER)) {
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;
    }
}
