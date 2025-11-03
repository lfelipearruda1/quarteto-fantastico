#ifndef SCREEN_GAME_H
#define SCREEN_GAME_H

#include "raylib.h"
#include "../states.h"

void ScreenGame_Init(void);
void ScreenGame_Update(GameState *state);
void ScreenGame_Draw(void);
void ScreenGame_Unload(void);

#endif
