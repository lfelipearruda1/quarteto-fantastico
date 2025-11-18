#ifndef GAME_H
#define GAME_H

typedef enum {
    STATE_LOGO,
    STATE_SELECT,
    STATE_RANKING,
    STATE_GAME
} GameState;

#define W 960
#define H 540

void InitGame(int level);
void UpdateGame(GameState *state);
void DrawGame(void);
void UnloadGame(void);

#endif

