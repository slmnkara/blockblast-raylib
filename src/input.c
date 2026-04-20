#include "input.h"
#include "board.h"
#include "score.h"

#define PANEL_PIECE_SCALE 25
#define DRAG_PIECE_SCALE  CELL_SIZE

void InputUpdate(GameState *state)
{
    if (AnimIsActive(&state->anims)) return;
    if (state->gameOver) return;

    Vector2 mouse = GetMousePosition();

    // --- DRAG START ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !state->isDragging) {
        for (int i = 0; i < 3; i++) {
            PieceSlot *slot = &state->slots[i];
            if (!slot->piece) continue;

            float pw = slot->piece->width * PANEL_PIECE_SCALE;
            float ph = slot->piece->height * PANEL_PIECE_SCALE;
            Rectangle rect = { slot->posX, slot->posY, pw, ph };

            if (CheckCollisionPointRec(mouse, rect)) {
                state->isDragging = true;
                state->dragSlotIndex = i;
                state->dragOffset.x = mouse.x - slot->posX;
                state->dragOffset.y = mouse.y - slot->posY;
                state->dragPos = mouse;
                break;
            }
        }
    }

    // --- DRAG MOVE ---
    if (state->isDragging) {
        state->dragPos = mouse;
    }

    // --- DRAG END (drop) ---
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state->isDragging) {
        state->isDragging = false;

        PieceSlot *slot = &state->slots[state->dragSlotIndex];
        Piece *piece = slot->piece;
        if (!piece) return;

        float pieceScreenX = state->dragPos.x - state->dragOffset.x;
        float pieceScreenY = state->dragPos.y - state->dragOffset.y;

        // Snap to nearest grid cell
        int gridCol = (int)((pieceScreenX - GRID_DRAW_X + CELL_SIZE / 2) / CELL_SIZE);
        int gridRow = (int)((pieceScreenY - GRID_DRAW_Y + CELL_SIZE / 2) / CELL_SIZE);

        if (BoardCanPlace(&state->board, piece, gridRow, gridCol)) {
            BoardPlace(&state->board, piece, gridRow, gridCol);

            bool clearedCells[GRID_SIZE][GRID_SIZE];
            int linesCleared = BoardClearLines(&state->board, clearedCells);

            if (linesCleared > 0) {
                state->combo++;
                state->score += ScoreCalculate(linesCleared, state->combo);
                AnimAddCleared(&state->anims, clearedCells);
            } else {
                state->combo = 0;
            }

            if (state->score > state->highScore)
                state->highScore = state->score;

            PieceFree(&slot->piece);

            if (AllSlotsEmpty(state->slots))
                GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);

            if (!BoardHasValidMove(&state->board, state->slots)) {
                state->gameOver = true;
                ScoreSaveHigh(state->highScore);
            }
        }
    }
}
