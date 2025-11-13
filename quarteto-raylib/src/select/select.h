#ifndef SELECT_H
#define SELECT_H

#include "game/game.h"
#include <stdbool.h>

void InitSelect(void);
void UpdateSelect(GameState *state);
void DrawSelect(void);
void UnloadSelect(void);
const char* GetPlayerName(void);
bool IsNameSaved(void);

#endif

