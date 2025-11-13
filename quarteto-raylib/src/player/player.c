#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player *playerList = NULL;

void AddPlayerToList(const char *name, int level) {
    Player *p = (Player*)malloc(sizeof(Player));
    if (!p) return;

    size_t len = strlen(name);
    p->name = (char*)malloc(len + 1);
    if (!p->name) {
        free(p);
        return;
    }
    memcpy(p->name, name, len + 1);

    p->levelChosen = level;
    p->next = NULL;

    p->next = playerList;
    playerList = p;
}

void SaveNameToFile(const char *name) {
    FILE *f = fopen("players.txt", "a");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
}

void FreePlayerList(void) {
    Player *curr = playerList;
    while (curr) {
        Player *next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }
    playerList = NULL;
}

