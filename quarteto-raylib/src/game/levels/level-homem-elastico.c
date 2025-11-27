#include "level-common.h"
#include <stdlib.h>

#define MAX_PROJECTILES 8
#define MAX_ENEMIES 20
#define MAX_OBSTACLES 28
#define MAP_LENGTH 12000
#define ENEMY_SPAWN_DISTANCE 500
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
} Projectile;


typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
    
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
    Texture2D playerIdleLeft;
    Texture2D playerRunning[4];
    Texture2D playerRunningLeft[4];
    Texture2D playerJumping;
    Texture2D playerJumpingLeft;
    Texture2D playerAttacking;
    Texture2D playerAttackingLeft;
    Texture2D playerCrouching;
    Texture2D playerCrouchingLeft;
    Texture2D enemyTexture;
} LevelElasticoData;

static void ShootProjectile(LevelElasticoData *data) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!data->projectiles[i].active) {
            if (data->player.facingRight) {
                data->projectiles[i].position = (Vector2){
                    data->player.position.x + data->player.width,
                    data->player.position.y + data->player.height / 2
                };
                data->projectiles[i].speed = 10.0f;
            } else {
                data->projectiles[i].position = (Vector2){
                    data->player.position.x,
                    data->player.position.y + data->player.height / 2
                };
                data->projectiles[i].speed = -10.0f;
            }
            data->projectiles[i].active = true;
            
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

    data->backgroundTexture = LoadTexture("Mapa Cidade/fundo_cidade.png");
    data->platformTexture = LoadTexture("Mapa Cidade/plataforma-cidade.png");
    data->obstacleTexture = LoadTexture("Mapa Cidade/obstaculo.png");
    data->heartTexture = LoadTexture("assets/coracao.png");
    data->enemyTexture = LoadTexture("assets/inimigo-coisa.png");

    data->playerIdle = LoadTexture("assets/homem-elastico/correndo-1.png");
    data->playerIdleLeft = LoadTexture("assets/homem-elastico/correndo-1 (1).png");
    data->playerRunning[0] = LoadTexture("assets/homem-elastico/correndo-1.png");
    data->playerRunning[1] = LoadTexture("assets/homem-elastico/correndo-2.png");
    data->playerRunning[2] = LoadTexture("assets/homem-elastico/correndo-3.png");
    data->playerRunning[3] = LoadTexture("assets/homem-elastico/correndo-4.png");
    data->playerRunningLeft[0] = LoadTexture("assets/homem-elastico/correndo-1 (1).png");
    data->playerRunningLeft[1] = LoadTexture("assets/homem-elastico/correndo-2 (1).png");
    data->playerRunningLeft[2] = LoadTexture("assets/homem-elastico/correndo-3 (1).png");
    data->playerRunningLeft[3] = LoadTexture("assets/homem-elastico/correndo-4 (1).png");
    data->playerJumping = LoadTexture("assets/homem-elastico/pulando-1.png");
    data->playerJumpingLeft = LoadTexture("assets/homem-elastico/pulando-1 (1).png");
    data->playerAttacking = LoadTexture("assets/homem-elastico/atacando-1.png");
    data->playerAttackingLeft = LoadTexture("assets/homem-elastico/atacando-1 (1).png");
    data->playerCrouching = LoadTexture("assets/homem-elastico/agachado-1.png");
    data->playerCrouchingLeft = LoadTexture("assets/homem-elastico/agachado-1 (1).png");

    data->player.position = (Vector2){ 100, GROUND_Y - 110 };
    data->player.velocity = (Vector2){ 0, 0 };
    data->player.onGround = true;
    data->player.health = 3;
    data->player.width = 90;
    data->player.height = 110;
    data->player.invulnerabilityTimer = 0;
    data->player.facingRight = true;
    
    data->currentAnimation = ANIM_IDLE;
    data->animFrame = 0;
    data->animTimer = 0;
    data->attackTimer = 0;
    data->isAttacking = false;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        data->projectiles[i].active = false;
        data->projectiles[i].position = (Vector2){0, 0};
        data->projectiles[i].speed = 0;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 600 + i * 550, GROUND_Y - 70 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 2.0f + (float)(rand() % 100) / 100.0f;
        data->enemies[i].width = 120;
        data->enemies[i].height = 120;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 450 + i * 420 + (float)(rand() % 150);
        float size = 75 + (float)(rand() % 50);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
        data->obstacles[i].textureIndex = 0;
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
        ShootProjectile(data);
    }

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);
    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

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

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            data->projectiles[i].position.x += data->projectiles[i].speed;
            
            if (data->projectiles[i].position.x > data->cameraX + W + 100 ||
                data->projectiles[i].position.x < data->cameraX - 100) {
                data->projectiles[i].active = false;
            }
        }
    }

    CommonActivateEnemiesNearPlayer(data->enemies, MAX_ENEMIES, data->player.position.x, ENEMY_SPAWN_DISTANCE);
    CommonUpdateEnemies(data->enemies, MAX_ENEMIES, data->cameraX);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!data->enemies[i].active) continue;
        
        Rectangle enemyBox = {
            data->enemies[i].position.x,
            data->enemies[i].position.y,
            data->enemies[i].width,
            data->enemies[i].height
        };
        
        for (int j = 0; j < MAX_PROJECTILES; j++) {
            if (!data->projectiles[j].active) continue;
            
            Rectangle projBox = {
                data->projectiles[j].position.x - 12,
                data->projectiles[j].position.y - 12,
                24,
                24
            };
            
            if (CheckCollisionRecs(enemyBox, projBox)) {
                data->enemies[i].active = false;
                data->enemies[i].position.x = -9999;
                data->projectiles[j].active = false;
                data->score += 100;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            if (CommonCheckPlayerEnemyCollision(&data->player, &data->enemies[i])) {
                CommonHandlePlayerDamage(&data->player, &data->enemies[i], &data->gameLost);
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
    CommonDrawObstacles(data->obstacles, MAX_OBSTACLES, &data->obstacleTexture, 1, data->cameraX);

    Texture2D currentTexture = data->playerIdle;
    if (data->player.facingRight) {
        switch (data->currentAnimation) {
            case ANIM_IDLE: 
                currentTexture = data->playerIdle;
                break;
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
    } else {
        switch (data->currentAnimation) {
            case ANIM_IDLE: 
                currentTexture = data->playerIdleLeft;
                break;
            case ANIM_RUNNING: 
                currentTexture = data->playerRunningLeft[data->animFrame % 4]; 
                break;
            case ANIM_JUMPING: 
                currentTexture = data->playerJumpingLeft; 
                break;
            case ANIM_ATTACKING: 
                currentTexture = data->playerAttackingLeft; 
                break;
        }
    }

    Rectangle source = {0, 0, (float)currentTexture.width, (float)currentTexture.height};
    Rectangle dest = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };
    DrawTexturePro(currentTexture, source, dest, (Vector2){0, 0}, 0, CommonGetPlayerColor(&data->player));

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            DrawCircle(
                (int)(data->projectiles[i].position.x - data->cameraX),
                (int)data->projectiles[i].position.y,
                12, SKYBLUE
            );
            DrawCircle(
                (int)(data->projectiles[i].position.x - data->cameraX),
                (int)data->projectiles[i].position.y,
                8, BLUE
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
    DrawText("WASD: Mover | W/ESPACO: Pular | E: Bolinha Azul", 10, H - 30, 16, LIGHTGRAY);

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
    if (data->playerIdle.id > 0) UnloadTexture(data->playerIdle);
    if (data->playerIdleLeft.id > 0) UnloadTexture(data->playerIdleLeft);
    
    for (int i = 0; i < 4; i++) {
        if (data->playerRunning[i].id > 0) UnloadTexture(data->playerRunning[i]);
        if (data->playerRunningLeft[i].id > 0) UnloadTexture(data->playerRunningLeft[i]);
    }
    if (data->playerJumping.id > 0) UnloadTexture(data->playerJumping);
    if (data->playerJumpingLeft.id > 0) UnloadTexture(data->playerJumpingLeft);
    if (data->playerAttacking.id > 0) UnloadTexture(data->playerAttacking);
    if (data->playerAttackingLeft.id > 0) UnloadTexture(data->playerAttackingLeft);
    if (data->playerCrouching.id > 0) UnloadTexture(data->playerCrouching);
    if (data->playerCrouchingLeft.id > 0) UnloadTexture(data->playerCrouchingLeft);
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
