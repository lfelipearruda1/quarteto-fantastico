#ifndef RANKING_H
#define RANKING_H

#include "../common.h"
#include "raylib.h"
#include <stdbool.h>

void SetCurrentPlayerName(const char *name, bool saved);
void LoadRanking(void);
void InitRanking(void);
void UpdateRanking(GameState *state);
void DrawRanking(void);
void UnloadRanking(void);

#endif

