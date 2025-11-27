#include "level-common.h"
#include <stdlib.h>
#include <math.h>

#define MAX_FIREBALLS 10
#define MAX_ENEMIES 12
#define MAX_OBSTACLES 16
#define MAP_LENGTH 7500
#define ENEMY_SPAWN_DISTANCE 600
#define FIREBALL_MAX_DISTANCE 400

typedef enum {
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_JUMPING,
    ANIM_ATTACKING
} AnimationState;

typedef struct {
    Vector2 position;
    Vector2 startPosition;
    bool active;
    float speed;
    int animFrame;
    float animTimer;
} Fireball;

typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
    
    Fireball fireballs[MAX_FIREBALLS];
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
    Texture2D playerJumping[3];
    Texture2D playerJumpingLeft[3];
    Texture2D playerAttacking[2];
    Texture2D playerAttackingLeft[2];
    Texture2D fireballTexture;
    Texture2D enemyTexture;
} LevelTochaData;

static void ShootFireball(LevelTochaData *data) {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!data->fireballs[i].active) {
            if (data->player.facingRight) {
                data->fireballs[i].position = (Vector2){
                    data->player.position.x + data->player.width,
                    data->player.position.y + data->player.height / 2
                };
                data->fireballs[i].speed = 12.0f;
            } else {
                data->fireballs[i].position = (Vector2){
                    data->player.position.x,
                    data->player.position.y + data->player.height / 2
                };
                data->fireballs[i].speed = -12.0f;
            }
            data->fireballs[i].startPosition = data->fireballs[i].position;
            data->fireballs[i].active = true;
            data->fireballs[i].animFrame = 0;
            data->fireballs[i].animTimer = 0;
            
            data->isAttacking = true;
            data->attackTimer = 0.3f;
            data->currentAnimation = ANIM_ATTACKING;
            data->animFrame = 0;
            break;
        }
    }
}

static void InitLevelTocha(Level *level) {
    LevelTochaData *data = (LevelTochaData*)malloc(sizeof(LevelTochaData));
    level->data = data;

    data->gameWon = false;
    data->gameLost = false;
    data->gameEndTimer = 0;
    data->score = 0;
    data->cameraX = 0;

    data->backgroundTexture = LoadTexture("assets/mapa-cidade/fundo-cidade.png");
    data->platformTexture = LoadTexture("assets/mapa-cidade/plataforma-cidade.png");
    data->obstacleTexture = LoadTexture("assets/mapa-cidade/obstaculo.png");
    data->heartTexture = LoadTexture("assets/coracao.png");
    data->enemyTexture = LoadTexture("assets/vilao/andando-1-espelhado.png");

    data->playerIdle = LoadTexture("assets/tocha-humana/parado-1-Photoroom.png");
    data->playerIdleLeft = LoadTexture("assets/tocha-humana/parado-1-Photoroom (1).png");
    data->playerRunning[0] = LoadTexture("assets/tocha-humana/correndo-2.png");
    data->playerRunning[1] = LoadTexture("assets/tocha-humana/correndo-3.png");
    data->playerRunning[2] = LoadTexture("assets/tocha-humana/correndo-2.png");
    data->playerRunningLeft[0] = LoadTexture("assets/tocha-humana/correndo-1.png");
    data->playerRunningLeft[1] = LoadTexture("assets/tocha-humana/correndo-2 (1).png");
    data->playerRunningLeft[2] = LoadTexture("assets/tocha-humana/correndo-3 (1).png");
    data->playerJumping[0] = LoadTexture("assets/tocha-humana/pulando-1-Photoroom.png");
    data->playerJumping[1] = LoadTexture("assets/tocha-humana/pulando-2-Photoroom.png");
    data->playerJumping[2] = LoadTexture("assets/tocha-humana/pulando-3-Photoroom.png");
    data->playerJumpingLeft[0] = LoadTexture("assets/tocha-humana/pulando-1-Photoroom (1).png");
    data->playerJumpingLeft[1] = LoadTexture("assets/tocha-humana/pulando-2-Photoroom (1).png");
    data->playerJumpingLeft[2] = LoadTexture("assets/tocha-humana/pulando-3-Photoroom (1).png");
    data->playerAttacking[0] = LoadTexture("assets/tocha-humana/atacando-1.png");
    data->playerAttacking[1] = LoadTexture("assets/tocha-humana/atacando-2-Photoroom.png");
    data->playerAttackingLeft[0] = LoadTexture("assets/tocha-humana/atacando-1 (1).png");
    data->playerAttackingLeft[1] = LoadTexture("assets/tocha-humana/atacando-2-Photoroom (1).png");
    data->fireballTexture = LoadTexture("assets/tocha-humana/bola-de-fogo-Photoroom.png");

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

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        data->fireballs[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 700 + i * 650, GROUND_Y - 110 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.5f + (float)(rand() % 70) / 100.0f;
        data->enemies[i].width = 90;
        data->enemies[i].height = 110;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 550 + i * 480 + (float)(rand() % 120);
        float size = 65 + (float)(rand() % 40);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
        data->obstacles[i].textureIndex = 0;
    }
}

static void UpdateLevelTocha(Level *level, GameState *state) {
    LevelTochaData *data = (LevelTochaData*)level->data;

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
        ShootFireball(data);
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
        int maxFrames = (data->currentAnimation == ANIM_RUNNING || data->currentAnimation == ANIM_JUMPING) ? 3 :
                        (data->currentAnimation == ANIM_ATTACKING) ? 2 : 1;
        if (data->animFrame >= maxFrames) data->animFrame = 0;
    }

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (data->fireballs[i].active) {
            data->fireballs[i].position.x += data->fireballs[i].speed;
            data->fireballs[i].animTimer += GetFrameTime();
            
            if (data->fireballs[i].animTimer >= 0.1f) {
                data->fireballs[i].animTimer = 0;
                data->fireballs[i].animFrame = (data->fireballs[i].animFrame + 1) % 4;
            }
            
            float distanceTraveled = fabs(data->fireballs[i].position.x - data->fireballs[i].startPosition.x);
            if (distanceTraveled >= FIREBALL_MAX_DISTANCE) {
                data->fireballs[i].active = false;
            }

            if (data->fireballs[i].position.x > data->cameraX + W + 50 ||
                data->fireballs[i].position.x < data->cameraX - 50) {
                data->fireballs[i].active = false;
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
        
        for (int j = 0; j < MAX_FIREBALLS; j++) {
            if (!data->fireballs[j].active) continue;
            
            Rectangle projBox = {
                data->fireballs[j].position.x - 20,
                data->fireballs[j].position.y - 20,
                40,
                40
            };
            
            if (CheckCollisionRecs(enemyBox, projBox)) {
                data->enemies[i].active = false;
                data->enemies[i].position.x = -9999;
                data->fireballs[j].active = false;
                data->score += 100;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            Rectangle playerRect = {
                data->player.position.x,
                data->player.position.y,
                data->player.width,
                data->player.height
            };
            Rectangle enemyRect = {
                data->enemies[i].position.x,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };
            
            if (CheckCollisionRecs(playerRect, enemyRect)) {
                // Check if player is jumping over enemy
                if (data->player.position.y + data->player.height < data->enemies[i].position.y + 20) {
                    // Player is above enemy, let enemy continue
                    continue;
                }
                
                // Enemy attacks player
                if (data->player.invulnerabilityTimer <= 0) {
                    data->player.health--;
                    data->player.invulnerabilityTimer = 1.5f;
                    // Push enemy back slightly
                    data->enemies[i].position.x += 20;
                    
                    if (data->player.health <= 0) {
                        data->gameLost = true;
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

static void DrawLevelTocha(Level *level) {
    LevelTochaData *data = (LevelTochaData*)level->data;

    ClearBackground((Color){30, 15, 50, 255});
    CommonDrawBackground(data->backgroundTexture, data->cameraX);
    CommonDrawPlatform(data->platformTexture, data->cameraX, MAP_LENGTH);
    CommonDrawObstacles(data->obstacles, MAX_OBSTACLES, &data->obstacleTexture, 1, data->cameraX);

    Texture2D currentTexture = data->playerIdle;
    if (data->player.facingRight) {
        switch (data->currentAnimation) {
            case ANIM_IDLE: currentTexture = data->playerIdle; break;
            case ANIM_RUNNING: currentTexture = data->playerRunning[data->animFrame % 3]; break;
            case ANIM_JUMPING: currentTexture = data->playerJumping[data->animFrame % 3]; break;
            case ANIM_ATTACKING: currentTexture = data->playerAttacking[data->animFrame % 2]; break;
        }
    } else {
        switch (data->currentAnimation) {
            case ANIM_IDLE: currentTexture = data->playerIdleLeft; break;
            case ANIM_RUNNING: currentTexture = data->playerRunningLeft[data->animFrame % 3]; break;
            case ANIM_JUMPING: currentTexture = data->playerJumpingLeft[data->animFrame % 3]; break;
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

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (data->fireballs[i].active) {
            Rectangle src = {0, 0, (float)data->fireballTexture.width, (float)data->fireballTexture.height};
            Rectangle dst = {data->fireballs[i].position.x - data->cameraX - 20, data->fireballs[i].position.y - 20, 40, 40};
            float rotation = data->fireballs[i].animFrame * 90.0f;
            DrawTexturePro(data->fireballTexture, src, dst, (Vector2){20, 20}, rotation, WHITE);
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
    DrawText("WASD: Mover | W/ESPACO: Pular | E: Lancar Bola de Fogo", 10, H - 30, 16, LIGHTGRAY);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelTocha(Level *level) {
    LevelTochaData *data = (LevelTochaData*)level->data;
    
    UnloadTexture(data->backgroundTexture);
    UnloadTexture(data->platformTexture);
    UnloadTexture(data->obstacleTexture);
    UnloadTexture(data->heartTexture);
    UnloadTexture(data->enemyTexture);
    UnloadTexture(data->playerIdle);
    UnloadTexture(data->playerIdleLeft);
    
    for (int i = 0; i < 3; i++) {
        UnloadTexture(data->playerRunning[i]);
        UnloadTexture(data->playerRunningLeft[i]);
        UnloadTexture(data->playerJumping[i]);
        UnloadTexture(data->playerJumpingLeft[i]);
    }
    for (int i = 0; i < 2; i++) {
        UnloadTexture(data->playerAttacking[i]);
        UnloadTexture(data->playerAttackingLeft[i]);
    }
    UnloadTexture(data->fireballTexture);
}

Level* CreateLevelTocha(void) {
    Level *level = (Level*)malloc(sizeof(Level));
    level->type = LEVEL_TOCHA_HUMANA;
    level->functions.init = InitLevelTocha;
    level->functions.update = UpdateLevelTocha;
    level->functions.draw = DrawLevelTocha;
    level->functions.unload = UnloadLevelTocha;
    level->data = NULL;
    level->functions.init(level);
    return level;
}
