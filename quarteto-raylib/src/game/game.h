#ifndef GAME_H
#define GAME_H

#include "../common.h"
#include "levels/level-common.h"

void InitGame(int level);
void UpdateGame(GameState *state);
void DrawGame(void);
void UnloadGame(void);

#endif

