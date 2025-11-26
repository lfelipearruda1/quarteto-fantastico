#include "raylib.h"
#include "logo/logo.h"
#include "select/select.h"
#include "ranking/ranking.h"
#include "game/game.h"
#include "player/player.h"

#define W 960
#define H 540

void FreeRanking(void);

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
