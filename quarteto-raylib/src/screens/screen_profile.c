#include "screen_profile.h"
#include <stdio.h>
#include <string.h>

static const int W = 960;
static const int H = 540;

static char playerName[32];
static int nameLen = 0;
static bool nameDirty = false;   // pra saber se precisa salvar

// retângulos dos botões
static Rectangle btnBack;
static Rectangle btnRanking;

// retângulos dos 4 “níveis/personagens”
static Rectangle card1, card2, card3, card4;
static int selectedCard = -1;

static void SavePlayerNameToFile(const char *name) {
    // garante que exista a pasta data/ aí no projeto
    FILE *f = fopen("data/player.txt", "w");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
}

void ScreenProfile_Init(void) {
    playerName[0] = '\0';
    nameLen = 0;
    nameDirty = false;

    btnBack = (Rectangle){ 20, 10, 120, 40 };
    btnRanking = (Rectangle){ W - 140, 10, 120, 40 };

    float topCards = H * 0.35f;
    float cardW = 180;
    float cardH = 160;
    float space = 20;
    float startX = (W - (4*cardW + 3*space)) * 0.5f;

    card1 = (Rectangle){ startX + (cardW + space) * 0, topCards, cardW, cardH };
    card2 = (Rectangle){ startX + (cardW + space) * 1, topCards, cardW, cardH };
    card3 = (Rectangle){ startX + (cardW + space) * 2, topCards, cardW, cardH };
    card4 = (Rectangle){ startX + (cardW + space) * 3, topCards, cardW, cardH };
}

void ScreenProfile_Update(GameState *state) {
    // 1) entrada de texto
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 125 && nameLen < (int)sizeof(playerName) - 1) {
            playerName[nameLen++] = (char)key;
            playerName[nameLen] = '\0';
            nameDirty = true;
        }
        key = GetCharPressed();
    }

    // backspace
    if (IsKeyPressed(KEY_BACKSPACE) && nameLen > 0) {
        nameLen--;
        playerName[nameLen] = '\0';
        nameDirty = true;
    }

    // ENTER salva o nome
    if (IsKeyPressed(KEY_ENTER) && nameLen > 0) {
        SavePlayerNameToFile(playerName);
        nameDirty = false;
    }

    Vector2 mouse = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // 2) botão VOLTAR
    if (mousePressed && CheckCollisionPointRec(mouse, btnBack)) {
        // se tiver nome não salvo, salva antes
        if (nameDirty && nameLen > 0) {
            SavePlayerNameToFile(playerName);
            nameDirty = false;
        }
        *state = STATE_MENU;
        return;
    }

    // 3) botão RANKING
    if (mousePressed && CheckCollisionPointRec(mouse, btnRanking)) {
        // salva o nome antes de ir pro ranking
        if (nameDirty && nameLen > 0) {
            SavePlayerNameToFile(playerName);
            nameDirty = false;
        }
        *state = STATE_RANKING;
        return;
    }

    // 4) clique nos cards
    if (mousePressed) {
        if (CheckCollisionPointRec(mouse, card1)) selectedCard = 0;
        else if (CheckCollisionPointRec(mouse, card2)) selectedCard = 1;
        else if (CheckCollisionPointRec(mouse, card3)) selectedCard = 2;
        else if (CheckCollisionPointRec(mouse, card4)) selectedCard = 3;
    }

    // se quiser: se já tem nome e já escolheu card, pode ir pro jogo ao apertar espaço
    if (IsKeyPressed(KEY_SPACE) && nameLen > 0 && selectedCard != -1) {
        *state = STATE_GAME;
    }
}

void ScreenProfile_Draw(void) {
    ClearBackground((Color){12, 16, 28, 255});

    // campo de texto em cima (centralizado)
    Rectangle textBox = { W*0.5f - 250, 80, 500, 40 };
    DrawRectangleRounded(textBox, 0.2f, 8, (Color){20, 26, 40, 255});
    DrawRectangleLinesEx(textBox, 2, RAYWHITE);

    DrawText("Digite seu nome:", (int)textBox.x, (int)textBox.y - 28, 18, RAYWHITE);
    DrawText(playerName[0] ? playerName : "_", (int)textBox.x + 10, (int)textBox.y + 10, 20, RAYWHITE);

    // botão voltar
    DrawRectangleRounded(btnBack, 0.2f, 8, (Color){50, 60, 80, 255});
    DrawText("Voltar", (int)btnBack.x + 20, (int)btnBack.y + 10, 18, WHITE);

    // botão ranking
    DrawRectangleRounded(btnRanking, 0.2f, 8, (Color){80, 50, 50, 255});
    DrawText("Ranking", (int)btnRanking.x + 10, (int)btnRanking.y + 10, 18, WHITE);

    // título dos cards
    DrawText("Selecione o personagem / nivel:", W/2 - 200, (int)(H*0.30f) - 30, 18, RAYWHITE);

    // desenha os 4 cards
    Rectangle cards[4] = { card1, card2, card3, card4 };
    const char *names[4] = { "Coisa", "Mulher Invisivel", "Tocha Humana", "Sr. Fantastico" };

    for (int i = 0; i < 4; i++) {
        Color base = (Color){30, 34, 46, 255};
        if (selectedCard == i) base = (Color){70, 130, 180, 255};
        DrawRectangleRounded(cards[i], 0.15f, 6, base);
        DrawRectangleLinesEx(cards[i], 2, RAYWHITE);

        int tw = MeasureText(names[i], 16);
        DrawText(names[i],
                 (int)(cards[i].x + (cards[i].width - tw)/2),
                 (int)(cards[i].y + cards[i].height/2 - 8),
                 16,
                 WHITE);
    }

    // dica
    DrawText("ENTER salva o nome • ESPACO vai pro jogo (se tiver nome e personagem)", 20, H - 28, 16, GRAY);
}

void ScreenProfile_Unload(void) {
    // nada por enquanto
}
