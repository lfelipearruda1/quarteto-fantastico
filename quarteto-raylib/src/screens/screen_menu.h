#ifndef SCREEN_MENU_H
#define SCREEN_MENU_H

#include "raylib.h"
#include "../states.h"

void ScreenMenu_Init(void);
void ScreenMenu_Update(GameState *state);
void ScreenMenu_Draw(void);
void ScreenMenu_Unload(void);

#endif
