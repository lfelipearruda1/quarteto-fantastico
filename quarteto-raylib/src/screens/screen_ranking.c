#include "screen_ranking.h"
#include <stdio.h>
#include <string.h>

static const int W = 960;
static const int H = 540;

static char currentPlayer[32];

void ScreenRanking_Init(void) {
    currentPlayer[0] = '\0';

    // tenta ler o nome salvo
    FILE *f = fopen("data/player.txt", "r");
    if (f) {
        fgets(currentPlayer, sizeof(currentPlayer), f);
        // tira \n
        size_t len = strlen(currentPlayer);
        if (len > 0 && currentPlayer[len-1] == '\n') {
            currentPlayer[len-1] = '\0';
        }
        fclose(f);
    }
}

void ScreenRanking_Update(GameState *state) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        *state = STATE_PROFILE;
    }
}

void ScreenRanking_Draw(void) {
    ClearBackground((Color){15, 15, 25, 255});

    DrawText("RANKING GERAL", W/2 - MeasureText("RANKING GERAL", 28)/2, 40, 28, RAYWHITE);

    DrawText("1. JogadorPro  -  9999 pts", 120, 120, 20, GOLD);
    DrawText("2. Alguem      -  8500 pts", 120, 150, 20, LIGHTGRAY);
    DrawText("3. Visitante   -  7000 pts", 120, 180, 20, LIGHTGRAY);

    DrawText("Seu nome:", 120, 240, 20, RAYWHITE);
    DrawText(currentPlayer[0] ? currentPlayer : "(nao cadastrado)", 120, 270, 20, SKYBLUE);

    DrawText("ESC para voltar", 20, H - 30, 18, GRAY);
}

void ScreenRanking_Unload(void) {
    // nada
}
