#include "raylib.h"
#include <stdlib.h>

#define W 960
#define H 540

typedef enum {
    STATE_LOGO,
    STATE_SELECT,
    STATE_RANKING,
    STATE_GAME
} GameState;

typedef enum {
    LEVEL_COISA,
    LEVEL_HOMEM_ELASTICO,
    LEVEL_MULHER_INVISIVEL,
    LEVEL_TOCHA_HUMANA
} LevelType;

typedef struct Level Level;

typedef struct {
    void (*init)(Level *level);
    void (*update)(Level *level, GameState *state);
    void (*draw)(Level *level);
    void (*unload)(Level *level);
} LevelFunctions;

struct Level {
    LevelType type;
    LevelFunctions functions;
    void *data;
};

Level* CreateLevelTocha(void);
Level* CreateLevelHomemElastico(void);
Level* CreateLevelMulherInvisivel(void);
Level* CreateLevelCoisa(void);
void DestroyLevel(Level *level);

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
