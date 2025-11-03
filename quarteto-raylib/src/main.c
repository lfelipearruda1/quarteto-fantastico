#include "raylib.h"
#include "states.h"
#include "screens/screen_menu.h"
#include "screens/screen_profile.h"
#include "screens/screen_ranking.h"
#include "screens/screen_game.h"

int main(void) {
    const int W = 960, H = 540;
    InitWindow(W, H, "Quarteto Fantastico");
    SetTargetFPS(60);

    GameState state = STATE_MENU;

    ScreenMenu_Init();
    ScreenProfile_Init();
    ScreenRanking_Init();
    ScreenGame_Init();

    while (!WindowShouldClose()) {

        switch (state) {
            case STATE_MENU:     ScreenMenu_Update(&state); break;
            case STATE_PROFILE:  ScreenProfile_Update(&state); break;
            case STATE_RANKING:  ScreenRanking_Update(&state); break;
            case STATE_GAME:     ScreenGame_Update(&state); break;
        }

        BeginDrawing();
        switch (state) {
            case STATE_MENU:     ScreenMenu_Draw(); break;
            case STATE_PROFILE:  ScreenProfile_Draw(); break;
            case STATE_RANKING:  ScreenRanking_Draw(); break;
            case STATE_GAME:     ScreenGame_Draw(); break;
        }
        EndDrawing();
    }

    ScreenMenu_Unload();
    ScreenProfile_Unload();
    ScreenRanking_Unload();
    ScreenGame_Unload();

    CloseWindow();
    return 0;
}
