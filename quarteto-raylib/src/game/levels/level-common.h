#ifndef LEVEL_COMMON_H
#define LEVEL_COMMON_H

#include "../../common.h"
#include "raylib.h"
#include <stdbool.h>

#define GROUND_Y 400

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

extern Player *playerList;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool onGround;
    int health;
    float width;
    float height;
    float invulnerabilityTimer;
    bool facingRight;
} BaseCharacter;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float width;
    float height;
} BaseEnemy;

typedef struct {
    Rectangle rect;
    bool active;
} BaseObstacle;

void UpdatePlayerScore(const char *name, int newScore);
void CommonUpdateInvulnerability(BaseCharacter *player);
Color CommonGetPlayerColor(BaseCharacter *player);
void CommonApplyGravity(BaseCharacter *player);
void CommonCheckGroundCollision(BaseCharacter *player, float groundY);
void CommonHandleJump(BaseCharacter *player);
void CommonUpdateHorizontalMovement(BaseCharacter *player);
void CommonUpdateCamera(float *cameraX, BaseCharacter *player);
void CommonClampPlayerToCamera(BaseCharacter *player, float cameraX);
void CommonCheckObstacleCollision(BaseCharacter *player, BaseObstacle *obstacles, int maxObstacles);
void CommonActivateEnemiesNearPlayer(BaseEnemy *enemies, int maxEnemies, float playerX, float spawnDistance);
void CommonUpdateEnemies(BaseEnemy *enemies, int maxEnemies, float cameraX);
bool CommonCheckPlayerEnemyCollision(BaseCharacter *player, BaseEnemy *enemy);
void CommonHandlePlayerDamage(BaseCharacter *player, BaseEnemy *enemy, bool *gameLost);
void CommonDrawHealthHearts(int health, Texture2D heartTexture);
void CommonDrawHUD(int score, float playerX, float mapLength);
void CommonDrawProgressBar(float progress, Color barColor);
void CommonDrawBackground(Texture2D backgroundTexture, float cameraX);
void CommonDrawPlatform(Texture2D platformTexture, float cameraX, int mapLength);
void CommonDrawObstacles(BaseObstacle *obstacles, int maxObstacles, Texture2D obstacleTexture, float cameraX);
void CommonDrawVictoryScreen(int score);
void CommonDrawGameOverScreen(int score);
void CommonHandleGameEnd(bool gameWon, bool gameLost, float *gameEndTimer, int score, GameState *state);
bool CommonCheckVictory(float playerX, float mapLength);
bool CommonCheckFall(float playerY);

#endif

