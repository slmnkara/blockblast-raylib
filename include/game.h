#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "board.h"
#include "piece.h"
#include "anim.h"

// Screen dimensions
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 800

// Layout constants
#define CELL_SIZE     50
#define GRID_DRAW_X   40   // (SCREEN_WIDTH - GRID_SIZE * CELL_SIZE) / 2
#define GRID_DRAW_Y   130
#define PANEL_Y       580  // Y position of the bottom piece panel

// Game screens
typedef enum {
    SCREEN_MENU,
    SCREEN_PLAY,
    SCREEN_GAMEOVER
} Screen;

// Central game state — passed by pointer to all modules
typedef struct {
    Screen currentScreen;
    Board board;
    PieceSlot slots[3];
    int score;
    int highScore;
    int combo;

    // Drag state
    bool isDragging;
    int dragSlotIndex;
    Vector2 dragPos;      // current drag position (screen coords)
    Vector2 dragOffset;   // offset from piece origin to mouse

    // Animation
    AnimQueue anims;

    bool gameOver;
} GameState;

// Initialize a new game state
void GameInit(GameState *state);

// Update game logic for one frame
void GameUpdate(GameState *state);

// Reset board and slots for a new game (keeps highscore)
void GameReset(GameState *state);

#endif // GAME_H
