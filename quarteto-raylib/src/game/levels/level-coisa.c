#include "level-common.h"
#include <stdlib.h>

#define MAX_ROCKS 3
#define MAX_ENEMIES 16
#define MAX_OBSTACLES 22
#define MAP_LENGTH 10000
#define MAX_ROCK_DISTANCE 400
#define ENEMY_SPAWN_DISTANCE 550

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
    float rotation;
} Rock;

typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
    
    Rock rocks[MAX_ROCKS];
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
    Texture2D playerRunning[3];
    Texture2D playerRunningLeft[3];
    Texture2D playerJumping;
    Texture2D playerJumpingLeft;
    Texture2D playerAttacking[2];
    Texture2D playerAttackingLeft[2];
    Texture2D playerCrouching;
    Texture2D playerCrouchingLeft;
    Texture2D rockTexture;
    Texture2D enemyTexture;
} Level1Data;

static void ShootRock(Level1Data *data) {
    for (int i = 0; i < MAX_ROCKS; i++) {
        if (!data->rocks[i].active) {
            if (data->player.facingRight) {
                data->rocks[i].position = (Vector2){
                    data->player.position.x + data->player.width,
                    data->player.position.y + data->player.height / 2
                };
                data->rocks[i].speed = 8.0f;
            } else {
                data->rocks[i].position = (Vector2){
                    data->player.position.x,
                    data->player.position.y + data->player.height / 2
                };
                data->rocks[i].speed = -8.0f;
            }
            data->rocks[i].startX = data->rocks[i].position.x;
            data->rocks[i].active = true;
            data->rocks[i].rotation = 0;
            
            data->isAttacking = true;
            data->attackTimer = 0.3f;
            data->currentAnimation = ANIM_ATTACKING;
            data->animFrame = 0;
            break;
        }
    }
}

static void InitLevel1(Level *level) {
    Level1Data *data = (Level1Data*)malloc(sizeof(Level1Data));
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

    data->playerIdle = LoadTexture("assets/coisa/parado.png");
    data->playerIdleLeft = LoadTexture("assets/coisa/parado (1).png");
    data->playerRunning[0] = LoadTexture("assets/coisa/correndo-1.png");
    data->playerRunning[1] = LoadTexture("assets/coisa/correndo-2.png");
    data->playerRunning[2] = LoadTexture("assets/coisa/correndo-3.png");
    data->playerRunningLeft[0] = LoadTexture("assets/coisa/correndo-1 (1).png");
    data->playerRunningLeft[1] = LoadTexture("assets/coisa/correndo-2 (1).png");
    data->playerRunningLeft[2] = LoadTexture("assets/coisa/correndo-3 (1).png");
    data->playerJumping = LoadTexture("assets/coisa/pulando-1.png");
    data->playerJumpingLeft = LoadTexture("assets/coisa/pulando-1.png");
    data->playerAttacking[0] = LoadTexture("assets/coisa/atacando-1.png");
    data->playerAttacking[1] = LoadTexture("assets/coisa/atacando-2.png");
    data->playerAttackingLeft[0] = LoadTexture("assets/coisa/atacando-1 (1).png");
    data->playerAttackingLeft[1] = LoadTexture("assets/coisa/atacando-2 (1).png");
    data->playerCrouching = LoadTexture("assets/coisa/agachando.png");
    data->playerCrouchingLeft = LoadTexture("assets/coisa/agachando (1).png");
    data->rockTexture = LoadTexture("assets/coisa/pedra.png");

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

    for (int i = 0; i < MAX_ROCKS; i++) {
        data->rocks[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 650 + i * 600, GROUND_Y - 80 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.8f + (float)(rand() % 90) / 100.0f;
        data->enemies[i].width = 80;
        data->enemies[i].height = 80;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 500 + i * 450 + (float)(rand() % 130);
        float size = 50 + (float)(rand() % 35);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
    }
}

static void UpdateLevel1(Level *level, GameState *state) {
    Level1Data *data = (Level1Data*)level->data;

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
        ShootRock(data);
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
        int maxFrames = (data->currentAnimation == ANIM_RUNNING) ? 3 :
                        (data->currentAnimation == ANIM_ATTACKING) ? 2 : 1;
        if (data->animFrame >= maxFrames) data->animFrame = 0;
    }

    for (int i = 0; i < MAX_ROCKS; i++) {
        if (data->rocks[i].active) {
            data->rocks[i].position.x += data->rocks[i].speed;
            data->rocks[i].rotation += (data->rocks[i].speed > 0) ? 5.0f : -5.0f;
            float distance = (data->rocks[i].speed > 0) ? 
                            (data->rocks[i].position.x - data->rocks[i].startX) : 
                            (data->rocks[i].startX - data->rocks[i].position.x);
            if (distance > MAX_ROCK_DISTANCE ||
                data->rocks[i].position.x > data->cameraX + W + 50 ||
                data->rocks[i].position.x < data->cameraX - 50) {
                data->rocks[i].active = false;
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

            for (int j = 0; j < MAX_ROCKS; j++) {
                if (data->rocks[j].active) {
                    Rectangle rockRect = {
                        data->rocks[j].position.x - 20,
                        data->rocks[j].position.y - 20,
                        40, 40
                    };
                    if (CheckCollisionRecs(rockRect, enemyRect)) {
                        data->enemies[i].active = false;
                        data->rocks[j].active = false;
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

static void DrawLevel1(Level *level) {
    Level1Data *data = (Level1Data*)level->data;

    ClearBackground(BLACK);
    CommonDrawBackground(data->backgroundTexture, data->cameraX);
    CommonDrawPlatform(data->platformTexture, data->cameraX, MAP_LENGTH);
    CommonDrawObstacles(data->obstacles, MAX_OBSTACLES, data->obstacleTexture, data->cameraX);

    Texture2D currentTexture = data->playerIdle;
    if (data->player.facingRight) {
        switch (data->currentAnimation) {
            case ANIM_IDLE: currentTexture = data->playerIdle; break;
            case ANIM_RUNNING: currentTexture = data->playerRunning[data->animFrame % 3]; break;
            case ANIM_JUMPING: currentTexture = data->playerJumping; break;
            case ANIM_ATTACKING: currentTexture = data->playerAttacking[data->animFrame % 2]; break;
        }
    } else {
        switch (data->currentAnimation) {
            case ANIM_IDLE: currentTexture = data->playerIdleLeft; break;
            case ANIM_RUNNING: currentTexture = data->playerRunningLeft[data->animFrame % 3]; break;
            case ANIM_JUMPING: currentTexture = data->playerJumpingLeft; break;
            case ANIM_ATTACKING: currentTexture = data->playerAttackingLeft[data->animFrame % 2]; break;
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

    for (int i = 0; i < MAX_ROCKS; i++) {
        if (data->rocks[i].active) {
            Rectangle src = {0, 0, (float)data->rockTexture.width, (float)data->rockTexture.height};
            Rectangle dst = {data->rocks[i].position.x - data->cameraX - 20, data->rocks[i].position.y - 20, 40, 40};
            DrawTexturePro(data->rockTexture, src, dst, (Vector2){20, 20}, data->rocks[i].rotation, WHITE);
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
    CommonDrawProgressBar(data->player.position.x / MAP_LENGTH, GREEN);
    DrawText("WASD: Mover | W/ESPACO: Pular | E/ENTER: Atirar Pedra", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevel1(Level *level) {
    Level1Data *data = (Level1Data*)level->data;
    
    if (data->backgroundTexture.id > 0) UnloadTexture(data->backgroundTexture);
    if (data->platformTexture.id > 0) UnloadTexture(data->platformTexture);
    if (data->obstacleTexture.id > 0) UnloadTexture(data->obstacleTexture);
    if (data->heartTexture.id > 0) UnloadTexture(data->heartTexture);
    if (data->enemyTexture.id > 0) UnloadTexture(data->enemyTexture);
    if (data->playerIdle.id > 0) UnloadTexture(data->playerIdle);
    if (data->playerIdleLeft.id > 0) UnloadTexture(data->playerIdleLeft);
    
    for (int i = 0; i < 3; i++) {
        if (data->playerRunning[i].id > 0) UnloadTexture(data->playerRunning[i]);
        if (data->playerRunningLeft[i].id > 0) UnloadTexture(data->playerRunningLeft[i]);
    }
    if (data->playerJumping.id > 0) UnloadTexture(data->playerJumping);
    if (data->playerJumpingLeft.id > 0) UnloadTexture(data->playerJumpingLeft);
    for (int i = 0; i < 2; i++) {
        if (data->playerAttacking[i].id > 0) UnloadTexture(data->playerAttacking[i]);
        if (data->playerAttackingLeft[i].id > 0) UnloadTexture(data->playerAttackingLeft[i]);
    }
    if (data->playerCrouching.id > 0) UnloadTexture(data->playerCrouching);
    if (data->playerCrouchingLeft.id > 0) UnloadTexture(data->playerCrouchingLeft);
    if (data->rockTexture.id > 0) UnloadTexture(data->rockTexture);
}

Level* CreateLevelCoisa(void) {
    Level *level = (Level*)malloc(sizeof(Level));
    level->type = LEVEL_COISA;
    level->functions.init = InitLevel1;
    level->functions.update = UpdateLevel1;
    level->functions.draw = DrawLevel1;
    level->functions.unload = UnloadLevel1;
    level->data = NULL;
    level->functions.init(level);
    return level;
}

void DestroyLevel(Level *level) {
    if (level) {
        if (level->functions.unload) level->functions.unload(level);
        if (level->data) free(level->data);
        free(level);
    }
}
