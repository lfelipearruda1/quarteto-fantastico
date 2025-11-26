#ifndef COMMON_H
#define COMMON_H

#define W 960
#define H 540

typedef enum {
    STATE_LOGO,
    STATE_SELECT,
    STATE_RANKING,
    STATE_GAME
} GameState;

typedef struct Player {
    char *name;
    int levelChosen;
    int score;
    struct Player *next;
} Player;

#endif

