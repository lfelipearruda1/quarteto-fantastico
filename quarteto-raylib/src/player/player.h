#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct Player {
    char *name;
    int levelChosen;
    struct Player *next;
} Player;

extern Player *playerList;

void AddPlayerToList(const char *name, int level);
void SaveNameToFile(const char *name);
void FreePlayerList(void);

#endif

