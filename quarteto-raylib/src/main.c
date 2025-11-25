#include "raylib.h"
#include <stdbool.h>

#define W 960
#define H 540

typedef enum {
    STATE_LOGO,
    STATE_SELECT,
    STATE_RANKING,
    STATE_GAME
} GameState;

void InitLogo(void);
void UpdateLogo(GameState *state);
void DrawLogo(void);
void UnloadLogo(void);

void InitSelect(void);
void UpdateSelect(GameState *state);
void DrawSelect(void);
void UnloadSelect(void);

void InitRanking(void);
void UpdateRanking(GameState *state);
void DrawRanking(void);
void FreeRanking(void);

void InitGame(int level);
void UpdateGame(GameState *state);
void DrawGame(void);
void UnloadGame(void);

void FreePlayerList(void);

int main(void) {
    InitWindow(W, H, "Quarteto Fantastico");
    SetTargetFPS(60);

    GameState state = STATE_LOGO;
    InitLogo();
    InitSelect();
    InitRanking();

    while (!WindowShouldClose()) {
        switch (state) {
            case STATE_LOGO:   UpdateLogo(&state); break;
            case STATE_SELECT: UpdateSelect(&state); break;
            case STATE_RANKING: UpdateRanking(&state); break;
            case STATE_GAME: UpdateGame(&state); break;
        }

        BeginDrawing();
        switch (state) {
            case STATE_LOGO:   DrawLogo(); break;
            case STATE_SELECT: DrawSelect(); break;
            case STATE_RANKING: DrawRanking(); break;
            case STATE_GAME: DrawGame(); break;
        }
        EndDrawing();
    }

    UnloadLogo();
    UnloadSelect();
    UnloadGame();
    FreePlayerList();
    FreeRanking();

    CloseWindow();
    return 0;
}
