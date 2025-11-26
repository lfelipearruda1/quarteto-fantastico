#include "level-common.h"
#include <stdlib.h>

#define MAX_PUNCHES 5
#define MAX_ENEMIES 10
#define MAX_OBSTACLES 12
#define MAP_LENGTH 6500
#define ENEMY_SPAWN_DISTANCE 550

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float startX;
} Punch;

typedef struct {
    BaseCharacter player;
    Punch punches[MAX_PUNCHES];
    BaseEnemy enemies[MAX_ENEMIES];
    BaseObstacle obstacles[MAX_OBSTACLES];
    float cameraX;
    bool gameWon;
    bool gameLost;
    float gameEndTimer;
    int score;
    Texture2D backgroundTexture;
    Texture2D playerTexture;
    Texture2D platformTexture;
    Texture2D obstacleTexture;
    Texture2D heartTexture;
} LevelElasticoData;

static void ShootPunch(LevelElasticoData *data) {
    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (!data->punches[i].active) {
            data->punches[i].position = (Vector2){
                data->player.position.x + data->player.width,
                data->player.position.y + data->player.height / 2
            };
            data->punches[i].startX = data->punches[i].position.x;
            data->punches[i].active = true;
            data->punches[i].speed = 9.0f;
            break;
        }
    }
}

static void InitLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)malloc(sizeof(LevelElasticoData));
    level->data = data;

    data->gameWon = false;
    data->gameLost = false;
    data->gameEndTimer = 0;
    data->score = 0;
    data->cameraX = 0;

    data->backgroundTexture = LoadTexture("assets/coisa/fundoicoisa.png");
    data->playerTexture = LoadTexture("assets/homem-elastico/foto-homem-elastico.png");
    data->platformTexture = LoadTexture("assets/coisa/plataforma-coisa.png");
    data->obstacleTexture = LoadTexture("assets/coisa/obstaculo-coisa.png");
    data->heartTexture = LoadTexture("assets/coracao.png");

    data->player.position = (Vector2){ 100, GROUND_Y - 110 };
    data->player.velocity = (Vector2){ 0, 0 };
    data->player.onGround = true;
    data->player.health = 3;
    data->player.width = 90;
    data->player.height = 110;
    data->player.invulnerabilityTimer = 0;

    for (int i = 0; i < MAX_PUNCHES; i++) {
        data->punches[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 700 + i * 680, GROUND_Y - 70 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.3f + (float)(rand() % 80) / 100.0f;
        data->enemies[i].width = 70;
        data->enemies[i].height = 70;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 550 + i * 530 + (float)(rand() % 120);
        float size = 45 + (float)(rand() % 25);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
    }
}

static void UpdateLevelElastico(Level *level, GameState *state) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;

    CommonHandleGameEnd(data->gameWon, data->gameLost, &data->gameEndTimer, data->score, state);
    if (data->gameWon || data->gameLost) return;

    CommonUpdateInvulnerability(&data->player);
    CommonUpdateHorizontalMovement(&data->player);
    CommonClampPlayerToCamera(&data->player, data->cameraX);
    CommonUpdateCamera(&data->cameraX, &data->player);
    CommonHandleJump(&data->player);

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        ShootPunch(data);
    }

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);
    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            data->punches[i].position.x += data->punches[i].speed;
            if (data->punches[i].position.x - data->punches[i].startX > 450 ||
                data->punches[i].position.x > data->cameraX + W + 50) {
                data->punches[i].active = false;
            }
        }
    }

    CommonActivateEnemiesNearPlayer(data->enemies, MAX_ENEMIES, data->player.position.x, ENEMY_SPAWN_DISTANCE);
    CommonUpdateEnemies(data->enemies, MAX_ENEMIES, data->cameraX);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            if (CommonCheckPlayerEnemyCollision(&data->player, &data->enemies[i])) {
                CommonHandlePlayerDamage(&data->player, &data->enemies[i], &data->gameLost);
            }

            Rectangle enemyRect = {
                data->enemies[i].position.x,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };

            for (int j = 0; j < MAX_PUNCHES; j++) {
                if (data->punches[j].active) {
                    Rectangle punchRect = {
                        data->punches[j].position.x - 15,
                        data->punches[j].position.y - 15,
                        30, 30
                    };
                    if (CheckCollisionRecs(punchRect, enemyRect)) {
                        data->enemies[i].active = false;
                        data->punches[j].active = false;
                        data->score += 100;
                    }
                }
            }
        }
    }

    if (CommonCheckVictory(data->player.position.x, MAP_LENGTH)) {
        data->gameWon = true;
        data->score += 500 + data->player.health * 100;
    }

    if (CommonCheckFall(data->player.position.y)) {
        data->gameLost = true;
    }
}

static void DrawLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;

    ClearBackground((Color){10, 20, 40, 255});
    CommonDrawBackground(data->backgroundTexture, data->cameraX);
    CommonDrawPlatform(data->platformTexture, data->cameraX, MAP_LENGTH);
    CommonDrawObstacles(data->obstacles, MAX_OBSTACLES, data->obstacleTexture, data->cameraX);

    Rectangle source = {0, 0, (float)data->playerTexture.width, (float)data->playerTexture.height};
    Rectangle dest = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };
    DrawTexturePro(data->playerTexture, source, dest, (Vector2){0, 0}, 0, CommonGetPlayerColor(&data->player));

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            float distance = data->punches[i].position.x - data->punches[i].startX;
            float maxDistance = 450;
            float width = 10 + (distance / maxDistance) * 40;

            DrawRectangle(
                (int)(data->punches[i].startX - data->cameraX),
                (int)(data->punches[i].position.y - 5),
                (int)width,
                10,
                BLUE
            );
            DrawCircle(
                (int)(data->punches[i].position.x - data->cameraX),
                (int)data->punches[i].position.y,
                15,
                SKYBLUE
            );
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            Rectangle enemyDrawRect = {
                data->enemies[i].position.x - data->cameraX,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };
            DrawRectangleRec(enemyDrawRect, PURPLE);
            DrawRectangleLinesEx(enemyDrawRect, 2, DARKPURPLE);
        }
    }

    CommonDrawHealthHearts(data->player.health, data->heartTexture);
    CommonDrawHUD(data->score, data->player.position.x, MAP_LENGTH);
    CommonDrawProgressBar(data->player.position.x / MAP_LENGTH, BLUE);
    DrawText("WASD: Mover | W/ESPACO: Pular | E/ENTER: Soco Elastico", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;
    
    if (data->backgroundTexture.id > 0) UnloadTexture(data->backgroundTexture);
    if (data->playerTexture.id > 0) UnloadTexture(data->playerTexture);
    if (data->platformTexture.id > 0) UnloadTexture(data->platformTexture);
    if (data->obstacleTexture.id > 0) UnloadTexture(data->obstacleTexture);
    if (data->heartTexture.id > 0) UnloadTexture(data->heartTexture);
}

Level* CreateLevelHomemElastico(void) {
    Level *level = (Level*)malloc(sizeof(Level));
    level->type = LEVEL_HOMEM_ELASTICO;
    level->functions.init = InitLevelElastico;
    level->functions.update = UpdateLevelElastico;
    level->functions.draw = DrawLevelElastico;
    level->functions.unload = UnloadLevelElastico;
    level->data = NULL;
    level->functions.init(level);
    return level;
}
