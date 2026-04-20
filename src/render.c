#include "render.h"
#include "score.h"
#include <stdio.h>

// Color palette for pieces (index 0 = empty/background)
static const Color PIECE_COLORS[] = {
    {40, 40, 60, 255},             // 0 = empty cell background
    {255, 87, 87, 255},            // 1 = red
    {255, 189, 46, 255},           // 2 = yellow
    {46, 204, 113, 255},           // 3 = green
    {52, 152, 219, 255},           // 4 = blue
    {155, 89, 182, 255},           // 5 = purple
    {255, 147, 51, 255},           // 6 = orange
    {26, 188, 156, 255},           // 7 = teal
};

#define BG_COLOR       (Color){22, 22, 36, 255}
#define GRID_BG_COLOR  (Color){30, 30, 48, 255}
#define GRID_LINE_CLR  (Color){50, 50, 70, 255}
#define PANEL_PIECE_SCALE 25

// --- Forward declarations ---
static void RenderBoard(GameState *state);
static void RenderPieceSlots(GameState *state);
static void RenderDraggedPiece(GameState *state);
static void RenderGhost(GameState *state);
static void RenderScore(GameState *state);
static void RenderMenu(GameState *state);
static void RenderGameOver(GameState *state);

void RenderFrame(GameState *state)
{
    BeginDrawing();
    ClearBackground(BG_COLOR);

    switch (state->currentScreen) {
        case SCREEN_MENU:
            RenderMenu(state);
            break;
        case SCREEN_PLAY:
            RenderScore(state);
            RenderBoard(state);
            RenderGhost(state);
            RenderPieceSlots(state);
            RenderDraggedPiece(state);
            break;
        case SCREEN_GAMEOVER:
            RenderScore(state);
            RenderBoard(state);
            RenderGameOver(state);
            break;
    }

    EndDrawing();
}

// ----- Board -----
static void RenderBoard(GameState *state)
{
    // Draw board background
    DrawRectangle(GRID_DRAW_X - 4, GRID_DRAW_Y - 4,
                  GRID_SIZE * CELL_SIZE + 8, GRID_SIZE * CELL_SIZE + 8,
                  GRID_LINE_CLR);

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int x = GRID_DRAW_X + c * CELL_SIZE;
            int y = GRID_DRAW_Y + r * CELL_SIZE;
            int val = state->board.cells[r][c];

            // Check if cell is being animated
            float alpha = AnimGetCellAlpha(&state->anims, r, c);
            if (alpha >= 0.0f) {
                // Animated cell: fade out
                Color clr = PIECE_COLORS[val > 0 ? val : 1];
                clr.a = (unsigned char)(alpha * 255);
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
            } else if (val != CELL_EMPTY) {
                // Normal filled cell
                Color clr = PIECE_COLORS[val];
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
                // Highlight edge
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, 3,
                    (Color){255,255,255,40});
            } else {
                // Empty cell
                DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2,
                    PIECE_COLORS[0]);
            }
        }
    }
}

// ----- Ghost preview -----
static void RenderGhost(GameState *state)
{
    if (!state->isDragging) return;

    PieceSlot *slot = &state->slots[state->dragSlotIndex];
    Piece *piece = slot->piece;
    if (!piece) return;

    float pieceScreenX = state->dragPos.x - state->dragOffset.x;
    float pieceScreenY = state->dragPos.y - state->dragOffset.y;

    int gridCol = (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE/2) / CELL_SIZE);
    int gridRow = (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE/2) / CELL_SIZE);

    if (!BoardCanPlace(&state->board, piece, gridRow, gridCol)) return;

    Color ghost = PIECE_COLORS[piece->colorIndex];
    ghost.a = 80;

    for (int r = 0; r < piece->height; r++) {
        for (int c = 0; c < piece->width; c++) {
            if (piece->shape[r][c] == 0) continue;
            int x = GRID_DRAW_X + (gridCol + c) * CELL_SIZE;
            int y = GRID_DRAW_Y + (gridRow + r) * CELL_SIZE;
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, ghost);
        }
    }
}

// ----- Piece slots (bottom panel) -----
static void RenderPieceSlots(GameState *state)
{
    // Panel background
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, SCREEN_HEIGHT - PANEL_Y + 10,
        (Color){30, 30, 50, 255});
    DrawRectangle(0, PANEL_Y - 10, SCREEN_WIDTH, 2, GRID_LINE_CLR);

    for (int i = 0; i < 3; i++) {
        // Skip the slot being dragged
        if (state->isDragging && state->dragSlotIndex == i) continue;

        PieceSlot *slot = &state->slots[i];
        if (!slot->piece) continue;

        Piece *p = slot->piece;
        Color clr = PIECE_COLORS[p->colorIndex];

        for (int r = 0; r < p->height; r++) {
            for (int c = 0; c < p->width; c++) {
                if (p->shape[r][c] == 0) continue;
                int x = (int)(slot->posX + c * PANEL_PIECE_SCALE);
                int y = (int)(slot->posY + r * PANEL_PIECE_SCALE);
                DrawRectangle(x + 1, y + 1,
                    PANEL_PIECE_SCALE - 2, PANEL_PIECE_SCALE - 2, clr);
            }
        }
    }
}

// ----- Dragged piece (follows mouse at grid scale) -----
static void RenderDraggedPiece(GameState *state)
{
    if (!state->isDragging) return;

    PieceSlot *slot = &state->slots[state->dragSlotIndex];
    Piece *p = slot->piece;
    if (!p) return;

    float baseX = state->dragPos.x - state->dragOffset.x;
    float baseY = state->dragPos.y - state->dragOffset.y;

    Color clr = PIECE_COLORS[p->colorIndex];
    clr.a = 200;

    for (int r = 0; r < p->height; r++) {
        for (int c = 0; c < p->width; c++) {
            if (p->shape[r][c] == 0) continue;
            int x = (int)(baseX + c * CELL_SIZE);
            int y = (int)(baseY + r * CELL_SIZE);
            DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, clr);
        }
    }
}

// ----- Score display -----
static void RenderScore(GameState *state)
{
    char buf[64];

    // Score
    sprintf(buf, "SKOR: %d", state->score);
    int w = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_WIDTH - w) / 2, 20, 28, WHITE);

    // High score
    sprintf(buf, "EN YUKSEK: %d", state->highScore);
    w = MeasureText(buf, 18);
    DrawText(buf, (SCREEN_WIDTH - w) / 2, 55, 18, (Color){150,150,170,255});

    // Combo
    if (state->combo > 1) {
        sprintf(buf, "COMBO x%d", state->combo);
        w = MeasureText(buf, 22);
        DrawText(buf, (SCREEN_WIDTH - w) / 2, 85, 22,
            (Color){255, 220, 50, 255});
    }
}

// ----- Menu screen -----
static void RenderMenu(GameState *state)
{
    const char *title = "BLOCK BLAST";
    int tw = MeasureText(title, 48);
    DrawText(title, (SCREEN_WIDTH - tw)/2, 200, 48, WHITE);

    const char *sub = "Baslamak icin ENTER'a bas";
    int sw = MeasureText(sub, 20);
    DrawText(sub, (SCREEN_WIDTH - sw)/2, 400, 20,
        (Color){150,150,170,255});

    char buf[64];
    sprintf(buf, "En Yuksek Skor: %d", state->highScore);
    int hw = MeasureText(buf, 20);
    DrawText(buf, (SCREEN_WIDTH - hw)/2, 320, 20,
        (Color){100,100,130,255});
}

// ----- Game Over screen -----
static void RenderGameOver(GameState *state)
{
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,160});

    const char *go = "OYUN BITTI";
    int gw = MeasureText(go, 44);
    DrawText(go, (SCREEN_WIDTH - gw)/2, 280, 44, (Color){255,87,87,255});

    char buf[64];
    sprintf(buf, "Skor: %d", state->score);
    int sw = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_WIDTH - sw)/2, 350, 28, WHITE);

    sprintf(buf, "En Yuksek: %d", state->highScore);
    sw = MeasureText(buf, 22);
    DrawText(buf, (SCREEN_WIDTH - sw)/2, 395, 22, (Color){150,150,170,255});

    const char *retry = "Tekrar oynamak icin ENTER'a bas";
    int rw = MeasureText(retry, 18);
    DrawText(retry, (SCREEN_WIDTH - rw)/2, 460, 18,
        (Color){100,100,130,255});
}
