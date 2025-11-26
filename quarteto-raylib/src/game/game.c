#include "game.h"
#include "levels/level-tocha.h"
#include "levels/level-homem-elastico.h"
#include "levels/level-mulher-invisivel.h"
#include "levels/level-coisa.h"
#include <stdlib.h>

static Level *currentLevel = NULL;

void InitGame(int level) {
    if (currentLevel != NULL) {
        DestroyLevel(currentLevel);
    }

    switch (level) {
        case 0:
            currentLevel = CreateLevelTocha();
            break;
        case 1:
            currentLevel = CreateLevelHomemElastico();
            break;
        case 2:
            currentLevel = CreateLevelMulherInvisivel();
            break;
        case 3:
            currentLevel = CreateLevelCoisa();
            break;
        default:
            currentLevel = CreateLevelTocha();
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
