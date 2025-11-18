#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include "../game.h"

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

Level* CreateLevelCoisa(void);
Level* CreateLevelHomemElastico(void);
Level* CreateLevelMulherInvisivel(void);
Level* CreateLevelTochaHumana(void);
void DestroyLevel(Level *level);

#endif

