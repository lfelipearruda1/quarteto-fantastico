#ifndef RANKING_H
#define RANKING_H

#include "game/game.h"
#include <stdbool.h>

typedef struct {
    char name[32];
    int score;
    int position;
} RankingPlayer;

extern RankingPlayer *rankingList;
extern int rankingCount;
extern int currentPlayerPosition;

void InitRanking(void);
void LoadRanking(void);
void UpdateRanking(GameState *state);
void DrawRanking(void);
void FreeRanking(void);
void SetCurrentPlayerName(const char *name, bool saved);

#endif

