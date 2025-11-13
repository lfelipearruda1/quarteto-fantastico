#include "ranking.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RankingPlayer *rankingList = NULL;
int rankingCount = 0;
int currentPlayerPosition = -1;

static char currentPlayerName[32] = {0};
static bool nameSaved = false;

void SetCurrentPlayerName(const char *name, bool saved) {
    strncpy(currentPlayerName, name, sizeof(currentPlayerName) - 1);
    currentPlayerName[sizeof(currentPlayerName) - 1] = '\0';
    nameSaved = saved;
}

void LoadRanking(void) {
    if (rankingList) {
        free(rankingList);
        rankingList = NULL;
        rankingCount = 0;
    }

    FILE *f = fopen("players.txt", "r");
    if (!f) {
        rankingCount = 0;
        return;
    }

    char line[64];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        if (strlen(line) > 0) {
            count++;
        }
    }
    rewind(f);

    if (count == 0) {
        fclose(f);
        return;
    }

    rankingList = (RankingPlayer*)malloc(count * sizeof(RankingPlayer));
    if (!rankingList) {
        fclose(f);
        return;
    }

    int idx = 0;
    while (fgets(line, sizeof(line), f) && idx < count) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        if (strlen(line) > 0) {
            strncpy(rankingList[idx].name, line, sizeof(rankingList[idx].name) - 1);
            rankingList[idx].name[sizeof(rankingList[idx].name) - 1] = '\0';
            rankingList[idx].position = idx + 1;
            idx++;
        }
    }
    fclose(f);
    rankingCount = idx;

    currentPlayerPosition = -1;
    if (nameSaved && strlen(currentPlayerName) > 0) {
        for (int i = 0; i < rankingCount; i++) {
            if (strcmp(rankingList[i].name, currentPlayerName) == 0) {
                currentPlayerPosition = i + 1;
                break;
            }
        }
    }
}

void InitRanking(void) {
    LoadRanking();
}

static Rectangle btnBackRanking;

void UpdateRanking(GameState *state) {
    btnBackRanking = (Rectangle){ 20, H - 50, 100, 35 };
    
    Vector2 mouse = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (mousePressed && CheckCollisionPointRec(mouse, btnBackRanking)) {
        *state = STATE_SELECT;
    }
}

void DrawRanking(void) {
    ClearBackground((Color){12, 16, 28, 255});

    const char *title = "RANKING GERAL";
    int titleWidth = MeasureText(title, 32);
    DrawText(title, W/2 - titleWidth/2, 30, 32, GOLD);

    Color backBtnColor = CheckCollisionPointRec(GetMousePosition(), btnBackRanking) ?
                         (Color){100, 100, 140, 255} : (Color){60, 60, 90, 255};
    DrawRectangleRounded(btnBackRanking, 0.3f, 6, backBtnColor);
    const char *backText = "Voltar";
    int backTextWidth = MeasureText(backText, 18);
    DrawText(backText, (int)(btnBackRanking.x + (btnBackRanking.width - backTextWidth) / 2), (int)(btnBackRanking.y + (btnBackRanking.height - 18) / 2), 18, WHITE);

    int startY = 100;
    int lineHeight = 35;
    int maxVisible = (H - startY - 80) / lineHeight;

    DrawText("Pos.", 50, startY, 20, GRAY);
    DrawText("Nome", 150, startY, 20, GRAY);

    DrawLine(40, startY + 25, W - 40, startY + 25, GRAY);

    int y = startY + 40;
    for (int i = 0; i < rankingCount && i < maxVisible; i++) {
        Color textColor = RAYWHITE;
        Color bgColor = (Color){20, 24, 36, 255};

        if (currentPlayerPosition > 0 && rankingList[i].position == currentPlayerPosition) {
            textColor = GOLD;
            bgColor = (Color){40, 30, 20, 255};
        }

        DrawRectangleRounded((Rectangle){40, y - 5, W - 80, lineHeight - 5}, 0.2f, 4, bgColor);

        char posStr[16];
        snprintf(posStr, sizeof(posStr), "%d", rankingList[i].position);
        DrawText(posStr, 50, y, 18, textColor);

        DrawText(rankingList[i].name, 150, y, 18, textColor);

        if (currentPlayerPosition > 0 && rankingList[i].position == currentPlayerPosition) {
            DrawText("<- VOCÊ", W - 150, y, 16, GOLD);
        }

        y += lineHeight;
    }

    if (currentPlayerPosition > 0 && currentPlayerPosition > maxVisible) {
        char posMsg[64];
        snprintf(posMsg, sizeof(posMsg), "Sua posicao: %d", currentPlayerPosition);
        DrawText(posMsg, W/2 - MeasureText(posMsg, 20)/2, H - 40, 20, GOLD);
    }

    if (rankingCount == 0) {
        DrawText("Nenhum jogador registrado ainda.", W/2 - 200, H/2, 20, GRAY);
    }
}

void FreeRanking(void) {
    if (rankingList) {
        free(rankingList);
        rankingList = NULL;
    }
    rankingCount = 0;
    currentPlayerPosition = -1;
}

