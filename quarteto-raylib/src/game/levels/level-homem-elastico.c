#include "level-common.h"
#include <stdlib.h>

#define MAX_PUNCHES 5
#define MAX_ENEMIES 10
#define MAX_OBSTACLES 12
#define MAP_LENGTH 6500
#define ENEMY_SPAWN_DISTANCE 550
#define MAX_PUNCH_DISTANCE 450

typedef enum {
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_JUMPING,
    ANIM_ATTACKING
} AnimationState;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float startX;
    float extension;
} Punch;

typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
    
    Punch punches[MAX_PUNCHES];
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
    Texture2D playerRunning[4];
    Texture2D playerJumping;
    Texture2D playerAttacking;
    Texture2D playerCrouching;
    Texture2D enemyTexture;
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
            data->punches[i].extension = 0;
            
            data->isAttacking = true;
            data->attackTimer = 0.3f;
            data->currentAnimation = ANIM_ATTACKING;
            data->animFrame = 0;
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
    data->platformTexture = LoadTexture("assets/coisa/plataforma-coisa.png");
    data->obstacleTexture = LoadTexture("assets/coisa/obstaculo-coisa.png");
    data->heartTexture = LoadTexture("assets/coracao.png");
    data->enemyTexture = LoadTexture("assets/coisa/inimigo-coisa.png");

    data->playerRunning[0] = LoadTexture("assets/homem-elastico/correndo-1.png");
    data->playerRunning[1] = LoadTexture("assets/homem-elastico/correndo-2.png");
    data->playerRunning[2] = LoadTexture("assets/homem-elastico/correndo-3.png");
    data->playerRunning[3] = LoadTexture("assets/homem-elastico/correndo-4.png");
    data->playerJumping = LoadTexture("assets/homem-elastico/pulando-1.png");
    data->playerAttacking = LoadTexture("assets/homem-elastico/atacando-1.png");
    data->playerCrouching = LoadTexture("assets/homem-elastico/agachado-1.png");

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

    if (IsKeyPressed(KEY_E)) {
        ShootPunch(data);
    }

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);

    if (!data->isAttacking) {
        if (!data->player.onGround) {
            if (data->currentAnimation != ANIM_JUMPING) {
                data->currentAnimation = ANIM_JUMPING;
                data->animFrame = 0;
                data->animTimer = 0;
            }
        } else if (data->player.velocity.x != 0) {
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
                      (data->currentAnimation == ANIM_JUMPING) ? 0.15f :
                      (data->currentAnimation == ANIM_ATTACKING) ? 0.15f : 0.1f;

    if (data->animTimer >= animSpeed) {
        data->animTimer = 0;
        data->animFrame++;
        int maxFrames = (data->currentAnimation == ANIM_RUNNING) ? 4 : 1;
        if (data->animFrame >= maxFrames) data->animFrame = 0;
    }

    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            data->punches[i].position.x += data->punches[i].speed;
            data->punches[i].extension = data->punches[i].position.x - data->punches[i].startX;

            if (data->punches[i].extension > MAX_PUNCH_DISTANCE ||
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

    Texture2D currentTexture = data->playerRunning[0];
    switch (data->currentAnimation) {
        case ANIM_IDLE: 
        case ANIM_RUNNING: 
            currentTexture = data->playerRunning[data->animFrame % 4]; 
            break;
        case ANIM_JUMPING: 
            currentTexture = data->playerJumping; 
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
    DrawTexturePro(currentTexture, source, dest, (Vector2){0, 0}, 0, CommonGetPlayerColor(&data->player));

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            float width = 10 + (data->punches[i].extension / MAX_PUNCH_DISTANCE) * 40;
            DrawRectangle(
                (int)(data->punches[i].startX - data->cameraX),
                (int)(data->punches[i].position.y - 5),
                (int)width, 10, BLUE
            );
            DrawCircle(
                (int)(data->punches[i].position.x - data->cameraX),
                (int)data->punches[i].position.y,
                15, SKYBLUE
            );
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
    CommonDrawProgressBar(data->player.position.x / MAP_LENGTH, BLUE);
    DrawText("WASD: Mover | W/ESPACO: Pular | E: Soco Elastico", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;
    
    if (data->backgroundTexture.id > 0) UnloadTexture(data->backgroundTexture);
    if (data->platformTexture.id > 0) UnloadTexture(data->platformTexture);
    if (data->obstacleTexture.id > 0) UnloadTexture(data->obstacleTexture);
    if (data->heartTexture.id > 0) UnloadTexture(data->heartTexture);
    if (data->enemyTexture.id > 0) UnloadTexture(data->enemyTexture);
    
    for (int i = 0; i < 4; i++) {
        if (data->playerRunning[i].id > 0) UnloadTexture(data->playerRunning[i]);
    }
    if (data->playerJumping.id > 0) UnloadTexture(data->playerJumping);
    if (data->playerAttacking.id > 0) UnloadTexture(data->playerAttacking);
    if (data->playerCrouching.id > 0) UnloadTexture(data->playerCrouching);
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
