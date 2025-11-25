#include "select.h"
#include "player/player.h"
#include "ranking/ranking.h"
#include "raylib.h"
#include <string.h>

static Rectangle cards[4];
static Texture2D cardImgs[4];
static int selectedCard = -1;

static char nameBuffer[32] = {0};
static int nameLen = 0;
static bool nameSaved = false;
static Rectangle btnSave;
static Rectangle btnRanking;

void InitSelect(void) {
    float top = H * 0.35f;
    float cardW = 180;
    float cardH = 160;
    float space = 20;
    float startX = (W - (4*cardW + 3*space)) * 0.5f;

    for (int i = 0; i < 4; i++) {
        cards[i] = (Rectangle){ startX + (cardW + space) * i, top, cardW, cardH };
    }

    cardImgs[0] = LoadTexture("assets/tocha-humana/foto-tocha-humana.png");
    cardImgs[1] = LoadTexture("assets/homem-elastico/foto-homem-elastico.png");
    cardImgs[2] = LoadTexture("assets/mulher-invisivel/foto-mulher-invisel.png");
    cardImgs[3] = LoadTexture("assets/coisa/foto-coisa.png");

    btnSave = (Rectangle){ W*0.5f + 140, 90, 120, 32 };
    btnRanking = (Rectangle){ W - 140, H - 50, 120, 40 };
}

void UpdateSelect(GameState *state) {
    if (!nameSaved) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && nameLen < (int)sizeof(nameBuffer) - 1) {
                nameBuffer[nameLen++] = (char)key;
                nameBuffer[nameLen] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && nameLen > 0) {
            nameLen--;
            nameBuffer[nameLen] = '\0';
        }
    }

    Vector2 mouse = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if ((mousePressed && CheckCollisionPointRec(mouse, btnSave) && !nameSaved && nameLen > 0) || (IsKeyPressed(KEY_ENTER) && nameLen > 0 && !nameSaved)) {
        SaveNameToFile(nameBuffer);
        AddPlayerToList(nameBuffer, -1);
        nameSaved = true;
        SetCurrentPlayerName(nameBuffer, true);
        LoadRanking();
    }

    if (mousePressed && CheckCollisionPointRec(mouse, btnRanking)) {
        LoadRanking();
        *state = STATE_RANKING;
    }

    if (mousePressed && nameSaved) {
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(mouse, cards[i])) {
                selectedCard = i;
                if (playerList != NULL) {
                    playerList->levelChosen = i;
                }
                if (i == 3) {
                    InitGame(i);
                    *state = STATE_GAME;
                }
            }
        }
    }
}

void DrawSelect(void) {
    ClearBackground((Color){12, 16, 28, 255});

    if (!nameSaved) {
        DrawText("Nome do jogador:", W*0.5f - 250, 60, 20, RAYWHITE);

        Rectangle nameBox = { W*0.5f - 250, 90, 380, 32 };
        DrawRectangleRounded(nameBox, 0.2f, 6, (Color){20, 26, 40, 255});
        DrawRectangleLinesEx(nameBox, 2, RAYWHITE);
        if (nameLen > 0) {
            DrawText(nameBuffer, (int)nameBox.x + 8, (int)nameBox.y + 8, 18, RAYWHITE);
        } else {
            DrawText("Digite seu nome...", (int)nameBox.x + 8, (int)nameBox.y + 8, 18, GRAY);
        }

        DrawRectangleRounded(btnSave, 0.2f, 6, (Color){80, 80, 120, 255});
        const char *saveText = "Salvar";
        int saveTextWidth = MeasureText(saveText, 18);
        DrawText(saveText, (int)(btnSave.x + (btnSave.width - saveTextWidth) / 2), (int)(btnSave.y + (btnSave.height - 18) / 2), 18, WHITE);
    } else {
        int fontSize = 28;
        int labelWidth = MeasureText("Jogador:", fontSize);
        int nameWidth = MeasureText(nameBuffer, fontSize);
        int totalWidth = labelWidth + 10 + nameWidth;
        int startX = (int)(W*0.5f - totalWidth/2);
        int yPos = 60;
        DrawText("Jogador:", startX, yPos, fontSize, RAYWHITE);
        DrawText(nameBuffer, startX + labelWidth + 10, yPos, fontSize, GOLD);
    }

    Color rankingBtnColor = CheckCollisionPointRec(GetMousePosition(), btnRanking) ?
                            (Color){100, 80, 140, 255} : (Color){60, 50, 90, 255};
    DrawRectangleRounded(btnRanking, 0.3f, 6, rankingBtnColor);
    const char *rankingText = "Ranking";
    int rankingTextWidth = MeasureText(rankingText, 20);
    DrawText(rankingText, (int)(btnRanking.x + (btnRanking.width - rankingTextWidth) / 2), (int)(btnRanking.y + (btnRanking.height - 20) / 2), 20, WHITE);

    const char *selectText = "Selecione o nivel/personagem:";
    int selectTextWidth = MeasureText(selectText, 20);
    DrawText(selectText, W/2 - selectTextWidth/2, 150, 20, RAYWHITE);
    if (!nameSaved) {
        DrawText("Primeiro salve um nome para poder escolher o nivel.", 20, H - 30, 16, RED);
    } else {
        DrawText("Clique em uma imagem para escolher.", 20, H - 30, 16, GRAY);
    }

    for (int i = 0; i < 4; i++) {
        Color border = (selectedCard == i) ? GOLD : RAYWHITE;
        DrawRectangleRounded(cards[i], 0.2f, 6, (Color){30, 34, 46, 255});

        if (cardImgs[i].id > 0) {
            Rectangle src = {0, 0, cardImgs[i].width, cardImgs[i].height};
            Rectangle dst = {cards[i].x + 10, cards[i].y + 10, cards[i].width - 20, cards[i].height - 20};
            DrawTexturePro(cardImgs[i], src, dst, (Vector2){0,0}, 0, WHITE);
        }

        DrawRectangleLinesEx(cards[i], 2, border);
    }
}

void UnloadSelect(void) {
    for (int i = 0; i < 4; i++) {
        if (cardImgs[i].id > 0) UnloadTexture(cardImgs[i]);
    }
}

const char* GetPlayerName(void) {
    return nameBuffer;
}

bool IsNameSaved(void) {
    return nameSaved;
}

