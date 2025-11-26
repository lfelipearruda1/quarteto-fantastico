#include "level-common.h"
#include <stdlib.h>

#define MAX_PROJECTILES 7
#define MAX_ENEMIES 14
#define MAX_OBSTACLES 18
#define MAP_LENGTH 9000
#define ENEMY_SPAWN_DISTANCE 650

typedef enum {
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_ATTACKING
} AnimationState;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    int animFrame;
    float animTimer;
} Projectile;

typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
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
    Texture2D platformTexture;
    Texture2D obstacleTexture;
    Texture2D heartTexture;
    Texture2D playerIdle;
    Texture2D playerRunning[4];
    Texture2D playerAttacking;
    Texture2D playerCrouching;
    Texture2D projectileTexture;
    Texture2D enemyTexture;
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
            data->projectiles[i].animFrame = 0;
            data->projectiles[i].animTimer = 0;
            
            data->isAttacking = true;
            data->attackTimer = 0.3f;
            data->currentAnimation = ANIM_ATTACKING;
            data->animFrame = 0;
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
    data->platformTexture = LoadTexture("assets/coisa/plataforma-coisa.png");
    data->obstacleTexture = LoadTexture("assets/coisa/obstaculo-coisa.png");
    data->heartTexture = LoadTexture("assets/coracao.png");
    data->enemyTexture = LoadTexture("assets/coisa/inimigo-coisa.png");

    data->playerIdle = LoadTexture("assets/mulher-invisivel/parado.png");
    data->playerRunning[0] = LoadTexture("assets/mulher-invisivel/correndo-1.png");
    data->playerRunning[1] = LoadTexture("assets/mulher-invisivel/correndo-2.png");
    data->playerRunning[2] = LoadTexture("assets/mulher-invisivel/correndo-3.png");
    data->playerRunning[3] = LoadTexture("assets/mulher-invisivel/correndo-4.png");
    data->playerAttacking = LoadTexture("assets/mulher-invisivel/atacando.png");
    data->playerCrouching = LoadTexture("assets/mulher-invisivel/agachado.png");
    data->projectileTexture = LoadTexture("assets/mulher-invisivel/bolha.png");

    data->player.position = (Vector2){ 100, GROUND_Y - 110 };
    data->player.velocity = (Vector2){ 0, 0 };
    data->player.onGround = true;
    data->player.health = 3;
    data->player.width = 90;
    data->player.height = 110;
    data->player.invulnerabilityTimer = 0;
    
    data->currentAnimation = ANIM_IDLE;
    data->animFrame = 0;
    data->animTimer = 0;
    data->attackTimer = 0;
    data->isAttacking = false;
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

    if (data->isAttacking) {
        data->attackTimer -= GetFrameTime();
        if (data->attackTimer <= 0) {
            data->isAttacking = false;
        }
    }

    CommonUpdateInvulnerability(&data->player);
    data->animTimer += GetFrameTime();
    
    CommonUpdateHorizontalMovement(&data->player);
    CommonClampPlayerToCamera(&data->player, data->cameraX);
    CommonUpdateCamera(&data->cameraX, &data->player);
    CommonHandleJump(&data->player);

    if (IsKeyPressed(KEY_Q) && !data->invisible) {
        data->invisible = true;
        data->invisibleTimer = 2.0f;
    }

    if (IsKeyPressed(KEY_E)) {
        ShootProjectile(data);
    }

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);

    if (!data->isAttacking) {
        if (data->player.velocity.x != 0) {
            if (data->currentAnimation != ANIM_RUNNING) {
                data->currentAnimation = ANIM_RUNNING;
                data->animFrame = 0;
                data->animTimer = 0;
            }
        } else {
            data->currentAnimation = ANIM_IDLE;
            data->animFrame = 0;
        }
    }

    float animSpeed = (data->currentAnimation == ANIM_RUNNING) ? 0.08f : 
                      (data->currentAnimation == ANIM_ATTACKING) ? 0.15f : 0.1f;

    if (data->animTimer >= animSpeed) {
        data->animTimer = 0;
        data->animFrame++;
        int maxFrames = (data->currentAnimation == ANIM_RUNNING) ? 4 : 1;
        if (data->animFrame >= maxFrames) data->animFrame = 0;
    }

    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            data->projectiles[i].position.x += data->projectiles[i].speed;
            
            data->projectiles[i].animTimer += GetFrameTime();
            if (data->projectiles[i].animTimer >= 0.1f) {
                data->projectiles[i].animTimer = 0;
                data->projectiles[i].animFrame = (data->projectiles[i].animFrame + 1) % 4;
            }

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

    Texture2D currentTexture = data->playerIdle;
    switch (data->currentAnimation) {
        case ANIM_IDLE: 
            currentTexture = data->playerIdle; 
            break;
        case ANIM_RUNNING: 
            currentTexture = data->playerRunning[data->animFrame % 4]; 
            break;
        case ANIM_ATTACKING: 
            currentTexture = data->playerAttacking; 
            break;
    }

    Rectangle source = {0, 0, (float)currentTexture.width, (float)currentTexture.height};
    Rectangle dest = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };
    
    Color playerColor = CommonGetPlayerColor(&data->player);
    if (data->invisible) {
        playerColor = Fade(playerColor, 0.3f);
    }
    DrawTexturePro(currentTexture, source, dest, (Vector2){0, 0}, 0, playerColor);

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            Rectangle src = {0, 0, (float)data->projectileTexture.width, (float)data->projectileTexture.height};
            Rectangle dst = {
                data->projectiles[i].position.x - data->cameraX - 20,
                data->projectiles[i].position.y - 20,
                40, 40
            };
            float rotation = data->projectiles[i].animFrame * 90.0f;
            DrawTexturePro(data->projectileTexture, src, dst, (Vector2){20, 20}, rotation, WHITE);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            Rectangle src = {0, 0, (float)data->enemyTexture.width, (float)data->enemyTexture.height};
            Rectangle dst = {data->enemies[i].position.x - data->cameraX, data->enemies[i].position.y, 
                           data->enemies[i].width, data->enemies[i].height};
            DrawTexturePro(data->enemyTexture, src, dst, (Vector2){0, 0}, 0, WHITE);
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
    if (data->platformTexture.id > 0) UnloadTexture(data->platformTexture);
    if (data->obstacleTexture.id > 0) UnloadTexture(data->obstacleTexture);
    if (data->heartTexture.id > 0) UnloadTexture(data->heartTexture);
    if (data->enemyTexture.id > 0) UnloadTexture(data->enemyTexture);
    if (data->playerIdle.id > 0) UnloadTexture(data->playerIdle);
    
    for (int i = 0; i < 4; i++) {
        if (data->playerRunning[i].id > 0) UnloadTexture(data->playerRunning[i]);
    }
    if (data->playerAttacking.id > 0) UnloadTexture(data->playerAttacking);
    if (data->playerCrouching.id > 0) UnloadTexture(data->playerCrouching);
    if (data->projectileTexture.id > 0) UnloadTexture(data->projectileTexture);
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
