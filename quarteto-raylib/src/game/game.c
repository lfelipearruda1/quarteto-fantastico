#include "game.h"
#include "raylib.h"
#include "levels/level.h"
#include <stdlib.h>

static Level *currentLevel = NULL;

void InitGame(int level) {
    if (currentLevel != NULL) {
        DestroyLevel(currentLevel);
    }

    switch (level) {
        case 0:
            currentLevel = CreateLevelCoisa();
            break;
        case 1:
            currentLevel = CreateLevelCoisa();
            break;
        default:
            currentLevel = CreateLevelCoisa();
            break;
    }
}

void UpdateGame(GameState *state) {
    if (currentLevel != NULL && currentLevel->functions.update != NULL) {
        currentLevel->functions.update(currentLevel, state);
    }
}

void DrawGame(void) {
    if (currentLevel != NULL && currentLevel->functions.draw != NULL) {
        currentLevel->functions.draw(currentLevel);
    }
}

void UnloadGame(void) {
    if (currentLevel != NULL) {
        DestroyLevel(currentLevel);
        currentLevel = NULL;
    }
}
