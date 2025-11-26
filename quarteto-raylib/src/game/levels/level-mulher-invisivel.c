#include "level-common.h"
#include <stdlib.h>

#define MAX_PROJECTILES 7
#define MAX_ENEMIES 14
#define MAX_OBSTACLES 18
#define MAP_LENGTH 9000
#define ENEMY_SPAWN_DISTANCE 650

typedef struct {
    Vector2 position;
    bool active;
    float speed;
} Projectile;

typedef struct {
    BaseCharacter player;
    bool invisible;
    float invisibleTimer;
    
    Projectile projectiles[MAX_PROJECTILES];
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
} LevelInvisivelData;

static void ShootProjectile(LevelInvisivelData *data) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!data->projectiles[i].active) {
            data->projectiles[i].position = (Vector2){
                data->player.position.x + data->player.width,
                data->player.position.y + data->player.height / 2
            };
            data->projectiles[i].active = true;
            data->projectiles[i].speed = 11.0f;
            break;
        }
    }
}

static void InitLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)malloc(sizeof(LevelInvisivelData));
    level->data = data;

    data->gameWon = false;
    data->gameLost = false;
    data->gameEndTimer = 0;
    data->score = 0;
    data->cameraX = 0;

    data->backgroundTexture = LoadTexture("assets/coisa/fundoicoisa.png");
    data->playerTexture = LoadTexture("assets/mulher-invisivel/foto-mulher-invisel.png");
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
    data->invisible = false;
    data->invisibleTimer = 0;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        data->projectiles[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 650 + i * 680, GROUND_Y - 85 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.8f + (float)(rand() % 100) / 100.0f;
        data->enemies[i].width = 85;
        data->enemies[i].height = 85;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 500 + i * 500 + (float)(rand() % 150);
        float size = 50 + (float)(rand() % 35);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
    }
}

static void UpdateLevelInvisivel(Level *level, GameState *state) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;

    CommonHandleGameEnd(data->gameWon, data->gameLost, &data->gameEndTimer, data->score, state);
    if (data->gameWon || data->gameLost) return;

    if (data->invisible) {
        data->invisibleTimer -= GetFrameTime();
        if (data->invisibleTimer <= 0) {
            data->invisible = false;
        }
    }

    CommonUpdateInvulnerability(&data->player);
    CommonUpdateHorizontalMovement(&data->player);

    if (IsKeyPressed(KEY_Q) && !data->invisible) {
        data->invisible = true;
        data->invisibleTimer = 2.0f;
    }

    CommonClampPlayerToCamera(&data->player, data->cameraX);
    CommonUpdateCamera(&data->cameraX, &data->player);
    CommonHandleJump(&data->player);

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        ShootProjectile(data);
    }

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);
    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            data->projectiles[i].position.x += data->projectiles[i].speed;
            if (data->projectiles[i].position.x > data->cameraX + W + 50) {
                data->projectiles[i].active = false;
            }
        }
    }

    CommonActivateEnemiesNearPlayer(data->enemies, MAX_ENEMIES, data->player.position.x, ENEMY_SPAWN_DISTANCE);
    CommonUpdateEnemies(data->enemies, MAX_ENEMIES, data->cameraX);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            if (!data->invisible && CommonCheckPlayerEnemyCollision(&data->player, &data->enemies[i])) {
                CommonHandlePlayerDamage(&data->player, &data->enemies[i], &data->gameLost);
            }

            Rectangle enemyRect = {
                data->enemies[i].position.x,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };

            for (int j = 0; j < MAX_PROJECTILES; j++) {
                if (data->projectiles[j].active) {
                    Rectangle projRect = {
                        data->projectiles[j].position.x - 15,
                        data->projectiles[j].position.y - 15,
                        30, 30
                    };
                    if (CheckCollisionRecs(projRect, enemyRect)) {
                        data->enemies[i].active = false;
                        data->projectiles[j].active = false;
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

static void DrawLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;

    ClearBackground((Color){5, 10, 20, 255});
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
    
    Color playerColor = WHITE;
    if (data->invisible) {
        playerColor = Fade(WHITE, 0.3f);
    } else {
        playerColor = CommonGetPlayerColor(&data->player);
    }
    DrawTexturePro(data->playerTexture, source, dest, (Vector2){0, 0}, 0, playerColor);

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            DrawCircle(
                (int)(data->projectiles[i].position.x - data->cameraX),
                (int)data->projectiles[i].position.y,
                15,
                VIOLET
            );
            DrawCircle(
                (int)(data->projectiles[i].position.x - data->cameraX),
                (int)data->projectiles[i].position.y,
                10,
                PINK
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
            DrawRectangleRec(enemyDrawRect, MAROON);
            DrawRectangleLinesEx(enemyDrawRect, 3, DARKBROWN);
        }
    }

    CommonDrawHealthHearts(data->player.health, data->heartTexture);
    CommonDrawHUD(data->score, data->player.position.x, MAP_LENGTH);
    
    if (data->invisible) {
        DrawText("INVISIVEL!", W - 150, 20, 20, VIOLET);
    } else {
        DrawText("Q: Invisivel", W - 150, 20, 18, GRAY);
    }

    CommonDrawProgressBar(data->player.position.x / MAP_LENGTH, VIOLET);
    DrawText("WASD: Mover | W: Pular | E: Atirar | Q: Invisivel", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;
    
    if (data->backgroundTexture.id > 0) UnloadTexture(data->backgroundTexture);
    if (data->playerTexture.id > 0) UnloadTexture(data->playerTexture);
    if (data->platformTexture.id > 0) UnloadTexture(data->platformTexture);
    if (data->obstacleTexture.id > 0) UnloadTexture(data->obstacleTexture);
    if (data->heartTexture.id > 0) UnloadTexture(data->heartTexture);
}

Level* CreateLevelMulherInvisivel(void) {
    Level *level = (Level*)malloc(sizeof(Level));
    level->type = LEVEL_MULHER_INVISIVEL;
    level->functions.init = InitLevelInvisivel;
    level->functions.update = UpdateLevelInvisivel;
    level->functions.draw = DrawLevelInvisivel;
    level->functions.unload = UnloadLevelInvisivel;
    level->data = NULL;
    level->functions.init(level);
    return level;
}
