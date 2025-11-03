#include "screen_game.h"

static const int W = 960;
static const int H = 540;

void ScreenGame_Init(void) {
    // se precisar carregar coisas do jogo, põe aqui
}

void ScreenGame_Update(GameState *state) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        *state = STATE_MENU;
    }
}

void ScreenGame_Draw(void) {
    ClearBackground(DARKBLUE);

    const char* txt = "JOGO RODANDO...";
    int fs = 28;
    DrawText(txt, W/2 - MeasureText(txt, fs)/2, H/2 - 14, fs, GOLD);
    DrawText("ESC para voltar ao menu", W/2 - 160, H/2 + 24, 18, LIGHTGRAY);
}

void ScreenGame_Unload(void) {
    // nada por enquanto
}
