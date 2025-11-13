#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STATE_LOGO,
    STATE_SELECT
} GameState;

static const int W = 960;
static const int H = 540;

static Texture2D logo;
static float fade = 0.0f;
static float timerLogo = 0.0f;

static Rectangle cards[4];
static Texture2D cardImgs[4];
static int selectedCard = -1;

static char nameBuffer[32] = {0};
static int nameLen = 0;
static bool nameSaved = false;
static Rectangle btnSave;

static int scores[4][3] = {
    {100, 80, 60},
    {120, 90, 50},
    {90,  70, 40},
    {110, 85, 55}
};

typedef struct Player {
    char *name;
    int levelChosen;
    struct Player *next;
} Player;

static Player *playerList = NULL;

void AddPlayerToList(const char *name, int level) {
    Player *p = (Player*)malloc(sizeof(Player));
    if (!p) return;

    size_t len = strlen(name);
    p->name = (char*)malloc(len + 1);
    if (!p->name) {
        free(p);
        return;
    }
    memcpy(p->name, name, len + 1);

    p->levelChosen = level;
    p->next = NULL;

    p->next = playerList;
    playerList = p;
}

void SaveNameToFile(const char *name) {
    FILE *f = fopen("players.txt", "a");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
}

void InitLogo(void) {
    logo = LoadTexture("assets/logo.png");
    fade = 0.0f;
    timerLogo = 0.0f;
}

void UpdateLogo(GameState *state) {
    float dt = GetFrameTime();
    timerLogo += dt;
    if (fade < 1.0f) fade += dt * 0.8f;

    if (timerLogo > 2.0f && IsKeyPressed(KEY_ENTER)) {
        *state = STATE_SELECT;
    }

    if (timerLogo > 5.0f) {
        *state = STATE_SELECT;
    }
}

void DrawLogo(void) {
    ClearBackground((Color){10, 18, 32, 255});
    DrawTexture(logo, W/2 - logo.width/2, H/2 - logo.height/2, Fade(WHITE, fade));
    if (timerLogo > 2.0f) {
        DrawText("PRESSIONE ENTER", W/2 - MeasureText("PRESSIONE ENTER", 20)/2, H*0.8f, 20, Fade(WHITE, fade));
    }
}

void InitSelect(void) {
    float top = H * 0.35f;
    float cardW = 180;
    float cardH = 160;
    float space = 20;
    float startX = (W - (4*cardW + 3*space)) * 0.5f;

    for (int i = 0; i < 4; i++) {
        cards[i] = (Rectangle){ startX + (cardW + space) * i, top, cardW, cardH };
    }

    cardImgs[0] = LoadTexture("assets/foto-coisa.png");
    cardImgs[1] = LoadTexture("assets/foto-tocha-humana.png");
    cardImgs[2] = LoadTexture("assets/foto-mulher-invisel.png");
    cardImgs[3] = LoadTexture("assets/foto-homem-elastico.png");

    btnSave = (Rectangle){ W*0.5f + 140, 90, 120, 32 };
}

void UpdateSelect(GameState *state) {
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

    Vector2 mouse = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if ((mousePressed && CheckCollisionPointRec(mouse, btnSave)) || (IsKeyPressed(KEY_ENTER) && nameLen > 0 && !nameSaved)) {
        SaveNameToFile(nameBuffer);
        AddPlayerToList(nameBuffer, -1);
        nameSaved = true;
    }

    if (mousePressed && nameSaved) {
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(mouse, cards[i])) {
                selectedCard = i;
                if (playerList != NULL) {
                    playerList->levelChosen = i;
                }
            }
        }
    }
}

void DrawSelect(void) {
    ClearBackground((Color){12, 16, 28, 255});

    DrawText("Nome do jogador:", W*0.5f - 250, 60, 20, RAYWHITE);

    Rectangle nameBox = { W*0.5f - 250, 90, 380, 32 };
    DrawRectangleRounded(nameBox, 0.2f, 6, (Color){20, 26, 40, 255});
    DrawRectangleLinesEx(nameBox, 2, nameSaved ? GREEN : RAYWHITE);
    DrawText(nameLen > 0 ? nameBuffer : "Digite seu nome...", (int)nameBox.x + 8, (int)nameBox.y + 8, 18, RAYWHITE);

    DrawRectangleRounded(btnSave, 0.2f, 6, nameSaved ? (Color){40, 120, 40, 255} : (Color){80, 80, 120, 255});
    DrawText("Salvar", (int)btnSave.x + 20, (int)btnSave.y + 6, 18, WHITE);

    DrawText("Selecione o nivel/personagem:", W/2 - 200, 150, 20, RAYWHITE);
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

int main(void) {
    InitWindow(W, H, "Quarteto Fantastico");
    SetTargetFPS(60);

    GameState state = STATE_LOGO;
    InitLogo();
    InitSelect();

    while (!WindowShouldClose()) {
        switch (state) {
            case STATE_LOGO:   UpdateLogo(&state); break;
            case STATE_SELECT: UpdateSelect(&state); break;
        }

        BeginDrawing();
        switch (state) {
            case STATE_LOGO:   DrawLogo(); break;
            case STATE_SELECT: DrawSelect(); break;
        }
        EndDrawing();
    }

    UnloadTexture(logo);
    for (int i = 0; i < 4; i++) {
        if (cardImgs[i].id > 0) UnloadTexture(cardImgs[i]);
    }

    Player *curr = playerList;
    while (curr) {
        Player *next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }

    CloseWindow();
    return 0;
}
