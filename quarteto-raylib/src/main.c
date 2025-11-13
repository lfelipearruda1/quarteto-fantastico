#include "raylib.h"
#include "game/game.h"
#include "logo/logo.h"
#include "select/select.h"
#include "ranking/ranking.h"
#include "player/player.h"

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
        }

        BeginDrawing();
        switch (state) {
            case STATE_LOGO:   DrawLogo(); break;
            case STATE_SELECT: DrawSelect(); break;
            case STATE_RANKING: DrawRanking(); break;
        }
        EndDrawing();
    }

    UnloadLogo();
    UnloadSelect();
    FreePlayerList();
    FreeRanking();

    CloseWindow();
    return 0;
}
