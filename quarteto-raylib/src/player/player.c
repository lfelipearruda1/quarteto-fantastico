#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Player *playerList = NULL;

void AddPlayerToList(const char *name, int level) {
    Player *p = (Player*)malloc(sizeof(Player));
    size_t len = strlen(name);
    p->name = (char*)malloc(len + 1);
    memcpy(p->name, name, len + 1);

    p->levelChosen = level;
    p->score = 0;
    p->next = NULL;

    p->next = playerList;
    playerList = p;
}

void SaveNameToFile(const char *name) {
    FILE *f = fopen("players.txt", "a");
    if (f) {
        fprintf(f, "%s 0\n", name);
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

void UpdatePlayerScore(const char *name, int newScore) {
    Player *curr = playerList;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            curr->score = newScore;
            break;
        }
        curr = curr->next;
    }

    FILE *f = fopen("players.txt", "r");
    if (!f) return;

    char line[128];
    int count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) > 1) count++;
    }
    rewind(f);

    if (count == 0) {
        fclose(f);
        return;
    }

    char **names = (char**)malloc(count * sizeof(char*));
    int *scores = (int*)malloc(count * sizeof(int));

    int idx = 0;
    while (fgets(line, sizeof(line), f) && idx < count) {
        char tempName[64];
        int tempScore;
        if (sscanf(line, "%63s %d", tempName, &tempScore) >= 1) {
            names[idx] = (char*)malloc(strlen(tempName) + 1);
            strcpy(names[idx], tempName);
            scores[idx] = tempScore;

            if (strcmp(tempName, name) == 0) {
                scores[idx] = newScore;
            }
            idx++;
        }
    }
    fclose(f);

    f = fopen("players.txt", "w");
    if (f) {
        for (int i = 0; i < idx; i++) {
            fprintf(f, "%s %d\n", names[i], scores[i]);
            free(names[i]);
        }
        fclose(f);
    }

    free(names);
    free(scores);
}
