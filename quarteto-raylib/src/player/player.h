#ifndef PLAYER_H
#define PLAYER_H

#include "../common.h"

extern Player *playerList;

void AddPlayerToList(const char *name, int level);
void SaveNameToFile(const char *name);
void FreePlayerList(void);
void UpdatePlayerScore(const char *name, int newScore);

#endif

