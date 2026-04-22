#include "game.h"
#include "score.h"
#include <string.h>

// Setting items for the settings screen
#define SETTING_COUNT 4
#define SETTING_SFX     0
#define SETTING_MUSIC   1
#define SETTING_RESTART 2
#define SETTING_QUIT    3

// Settings gear icon hit area (top-right corner)
#define GEAR_X       (SCREEN_WIDTH - 50)
#define GEAR_Y       10
#define GEAR_SIZE    36

// Menu button layout
#define MENU_BTN_W      260
#define MENU_BTN_H      55
#define MENU_BTN_X      ((SCREEN_WIDTH - MENU_BTN_W) / 2)
#define MENU_STD_Y      340
#define MENU_ADV_Y      420

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
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;
    state->selectedSetting = 0;

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
    state->floatTexts.count = 0;
    state->particles.count = 0;
    state->banner.active = false;

    GenerateRandomPieces(state->slots, PANEL_Y, SCREEN_WIDTH);
}

void GameUpdate(GameState *state)
{
    float dt = GetFrameTime();

    // Always update visual effects (they should animate on all screens)
    FloatTextUpdate(&state->floatTexts, dt);
    ParticleUpdate(&state->particles, dt);

    // Banner timer
    if (state->banner.active) {
        state->banner.timer -= dt;
        if (state->banner.timer <= 0.0f) {
            state->banner.active = false;
        }
    }

    Vector2 mouse = GetMousePosition();

    switch (state->currentScreen) {
        case SCREEN_MENU: {
            // Standard Mode button
            Rectangle stdBtn = { MENU_BTN_X, MENU_STD_Y, MENU_BTN_W, MENU_BTN_H };
            // Adventure Mode button
            Rectangle advBtn = { MENU_BTN_X, MENU_ADV_Y, MENU_BTN_W, MENU_BTN_H };

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, stdBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    state->currentScreen = SCREEN_PLAY;
                    GameReset(state);
                } else if (CheckCollisionPointRec(mouse, advBtn)) {
                    SoundPlayMenuClick(&state->sound);
                    // Adventure mode not yet implemented — do nothing
                }
            }
            break;
        }

        case SCREEN_PLAY: {
            // Update animations
            AnimUpdate(&state->anims, dt);

            // Open settings by clicking the gear icon (top-right)
            Rectangle gearRect = { GEAR_X, GEAR_Y, GEAR_SIZE, GEAR_SIZE };
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(mouse, gearRect)) {
                state->currentScreen = SCREEN_SETTINGS;
                state->selectedSetting = 0;
            }

            // Check game over transition
            if (state->gameOver) {
                SoundPlayLose(&state->sound);
                state->currentScreen = SCREEN_GAMEOVER;
            }
            break;
        }

        case SCREEN_SETTINGS: {
            // Navigate settings with keyboard
            if (IsKeyPressed(KEY_UP)) {
                state->selectedSetting--;
                if (state->selectedSetting < 0)
                    state->selectedSetting = SETTING_COUNT - 1;
                SoundPlayMenuClick(&state->sound);
            }
            if (IsKeyPressed(KEY_DOWN)) {
                state->selectedSetting++;
                if (state->selectedSetting >= SETTING_COUNT)
                    state->selectedSetting = 0;
                SoundPlayMenuClick(&state->sound);
            }

            // Hover highlight: update selectedSetting based on mouse position
            for (int i = 0; i < SETTING_COUNT; i++) {
                Rectangle itemRect = { SCREEN_WIDTH / 2 - 120, 270 + i * 55 - 5, 240, 40 };
                if (CheckCollisionPointRec(mouse, itemRect)) {
                    state->selectedSetting = i;
                }
            }

            // Helper macro for executing a setting action
            #define EXECUTE_SETTING(idx) do { \
                SoundPlayMenuClick(&state->sound); \
                switch (idx) { \
                    case SETTING_SFX: SoundToggleSfx(&state->sound); break; \
                    case SETTING_MUSIC: SoundToggleMusic(&state->sound); break; \
                    case SETTING_RESTART: state->currentScreen = SCREEN_PLAY; GameReset(state); break; \
                    case SETTING_QUIT: state->currentScreen = SCREEN_MENU; break; \
                } \
            } while(0)

            // Select setting with ENTER
            if (IsKeyPressed(KEY_ENTER)) {
                EXECUTE_SETTING(state->selectedSetting);
            }

            // Click on setting items
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < SETTING_COUNT; i++) {
                    Rectangle itemRect = { SCREEN_WIDTH / 2 - 120, 270 + i * 55 - 5, 240, 40 };
                    if (CheckCollisionPointRec(mouse, itemRect)) {
                        EXECUTE_SETTING(i);
                        break;
                    }
                }
            }

            #undef EXECUTE_SETTING

            // Back to game with ESC
            if (IsKeyPressed(KEY_ESCAPE)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_PLAY;
            }
            break;
        }

        case SCREEN_GAMEOVER:
            if (IsKeyPressed(KEY_ENTER)) {
                SoundPlayMenuClick(&state->sound);
                state->currentScreen = SCREEN_PLAY;
                GameReset(state);
            }
            break;
    }
}
