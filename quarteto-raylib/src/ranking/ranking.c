#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 960
#define H 540
#define MAX_RANKING_SIZE 1000

typedef struct {
    char name[32];
    int score;
} RankingEntry;

static RankingEntry *rankingData = NULL;
static int totalPlayers = 0;
static char currentPlayerName[32] = {0};
static bool hasCurrentPlayer = false;
static Rectangle btnBack;

void SetCurrentPlayerName(const char *name, bool saved) {
    if (name && saved) {
        strncpy(currentPlayerName, name, sizeof(currentPlayerName) - 1);
        currentPlayerName[sizeof(currentPlayerName) - 1] = '\0';
        hasCurrentPlayer = true;
    } else {
        currentPlayerName[0] = '\0';
        hasCurrentPlayer = false;
    }
}

static int CompareScores(const void *a, const void *b) {
    const RankingEntry *entryA = (const RankingEntry*)a;
    const RankingEntry *entryB = (const RankingEntry*)b;
    return entryB->score - entryA->score;
}

void LoadRanking(void) {
    if (rankingData) {
        free(rankingData);
        rankingData = NULL;
    }
    totalPlayers = 0;

    FILE *f = fopen("players.txt", "r");
    if (!f) return;

    rankingData = (RankingEntry*)malloc(MAX_RANKING_SIZE * sizeof(RankingEntry));

    char line[128];
    while (fgets(line, sizeof(line), f) && totalPlayers < MAX_RANKING_SIZE) {
        char name[64];
        int score = 0;
        
        if (sscanf(line, "%63s %d", name, &score) >= 1) {
            strncpy(rankingData[totalPlayers].name, name, sizeof(rankingData[totalPlayers].name) - 1);
            rankingData[totalPlayers].name[sizeof(rankingData[totalPlayers].name) - 1] = '\0';
            rankingData[totalPlayers].score = score;
            totalPlayers++;
        }
    }
    fclose(f);

    qsort(rankingData, totalPlayers, sizeof(RankingEntry), CompareScores);
}

void InitRanking(void) {
    btnBack = (Rectangle){ 20, H - 60, 120, 40 };
    LoadRanking();
}

void UpdateRanking(GameState *state) {
    Vector2 mouse = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, btnBack)) {
            *state = STATE_SELECT;
        }
    }
}

void DrawRanking(void) {
    ClearBackground((Color){12, 16, 28, 255});

    const char *title = "RANKING - TOP JOGADORES";
    int titleWidth = MeasureText(title, 36);
    DrawText(title, W/2 - titleWidth/2, 20, 36, GOLD);

    Color backBtnColor = CheckCollisionPointRec(GetMousePosition(), btnBack) ? 
                         (Color){80, 80, 120, 255} : (Color){50, 50, 80, 255};
    DrawRectangleRounded(btnBack, 0.3f, 6, backBtnColor);
    const char *backText = "VOLTAR";
    int backWidth = MeasureText(backText, 20);
    DrawText(backText, (int)(btnBack.x + (btnBack.width - backWidth) / 2), 
             (int)(btnBack.y + (btnBack.height - 20) / 2), 20, WHITE);

    if (totalPlayers == 0) {
        const char *msg = "Nenhum jogador cadastrado ainda!";
        int msgWidth = MeasureText(msg, 24);
        DrawText(msg, W/2 - msgWidth/2, H/2, 24, GRAY);
        return;
    }

    int startY = 80;
    int lineHeight = 35;
    int maxDisplay = 10;

    DrawText("POS", 60, startY, 20, LIGHTGRAY);
    DrawText("JOGADOR", 150, startY, 20, LIGHTGRAY);
    DrawText("PONTOS", W - 200, startY, 20, LIGHTGRAY);
    DrawLine(50, startY + 28, W - 50, startY + 28, GRAY);

    int currentPlayerPos = -1;
    if (hasCurrentPlayer) {
        for (int i = 0; i < totalPlayers; i++) {
            if (strcmp(rankingData[i].name, currentPlayerName) == 0) {
                currentPlayerPos = i + 1;
                break;
            }
        }
    }

    int y = startY + 40;
    for (int i = 0; i < totalPlayers && i < maxDisplay; i++) {
        bool isCurrentPlayer = (currentPlayerPos > 0 && (i + 1) == currentPlayerPos);
        
        Color bgColor = isCurrentPlayer ? (Color){60, 45, 20, 200} : (Color){25, 30, 45, 150};
        Color textColor = isCurrentPlayer ? GOLD : RAYWHITE;
        
        DrawRectangleRounded((Rectangle){50, y - 8, W - 100, lineHeight - 5}, 0.2f, 6, bgColor);
        
        if (isCurrentPlayer) {
            DrawRectangleLinesEx((Rectangle){50, y - 8, W - 100, lineHeight - 5}, 2, GOLD);
        }

        DrawText(TextFormat("%d°", i + 1), 60, y, 20, textColor);
        DrawText(rankingData[i].name, 150, y, 20, textColor);
        DrawText(TextFormat("%d pts", rankingData[i].score), W - 200, y, 20, textColor);

        if (isCurrentPlayer) {
            DrawText("← VOCE", W - 110, y + 2, 16, GOLD);
        }

        y += lineHeight;
    }
}

void FreeRanking(void) {
    if (rankingData) {
        free(rankingData);
        rankingData = NULL;
    }
    totalPlayers = 0;
    hasCurrentPlayer = false;
}
