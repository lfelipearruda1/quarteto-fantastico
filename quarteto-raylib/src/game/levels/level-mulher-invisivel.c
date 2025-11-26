#include "level-common.h"
#include <stdlib.h>
#include <math.h>

#define MAX_PROJECTILES 7
#define MAX_ENEMIES 8
#define MAX_OBSTACLES 10
#define MAP_LENGTH 5000
#define ENEMY_SPAWN_DISTANCE 700
#define PROJECTILE_MAX_DISTANCE 400

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
} Projectile;

typedef enum {
    ENEMY_WALKING,
    ENEMY_ATTACKING
} EnemyState;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float width;
    float height;
    EnemyState state;
    int animFrame;
    float animTimer;
    float stateTimer;
} Enemy;

typedef struct {
    BaseCharacter player;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
    bool invisible;
    float invisibleTimer;
    int invisibleUses;

    Projectile projectiles[MAX_PROJECTILES];
    Enemy enemies[MAX_ENEMIES];
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
    Texture2D projectileTexture;
    Texture2D enemyWalking[2];
    Texture2D enemyAttacking;
} LevelInvisivelData;

static void ShootProjectile(LevelInvisivelData *data) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!data->projectiles[i].active) {
            if (data->player.facingRight) {
                data->projectiles[i].position = (Vector2){
                    data->player.position.x + data->player.width,
                    data->player.position.y + data->player.height / 2
                };
                data->projectiles[i].speed = 11.0f;
            } else {
                data->projectiles[i].position = (Vector2){
                    data->player.position.x,
                    data->player.position.y + data->player.height / 2
                };
                data->projectiles[i].speed = -11.0f;
            }
            data->projectiles[i].startPosition = data->projectiles[i].position;
            data->projectiles[i].active = true;
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

    data->backgroundTexture = LoadTexture("assets/mapa-cidade/fundo-cidade.png");
    data->platformTexture   = LoadTexture("assets/mapa-cidade/plataforma-cidade.png");
    data->obstacleTexture   = LoadTexture("assets/mapa-cidade/obstaculo.png");
    data->heartTexture      = LoadTexture("assets/coracao.png");

    data->enemyWalking[0]   = LoadTexture("assets/vilão/andando-1-espelhado.png");
    data->enemyWalking[1]   = LoadTexture("assets/vilão/andando-2-espelhado.png");
    data->enemyAttacking    = LoadTexture("assets/vilão/luta-espelhado.png");

    data->playerIdle        = LoadTexture("assets/mulher-invisivel/parado.png");
    data->playerIdleLeft    = LoadTexture("assets/mulher-invisivel/parado (1).png");
    data->playerRunning[0]  = LoadTexture("assets/mulher-invisivel/correndo-1.png");
    data->playerRunning[1]  = LoadTexture("assets/mulher-invisivel/correndo-2.png");
    data->playerRunning[2]  = LoadTexture("assets/mulher-invisivel/correndo-3.png");
    data->playerRunning[3]  = LoadTexture("assets/mulher-invisivel/correndo-4.png");
    data->playerRunningLeft[0] = LoadTexture("assets/mulher-invisivel/correndo-1 (1).png");
    data->playerRunningLeft[1] = LoadTexture("assets/mulher-invisivel/correndo-2 (1).png");
    data->playerRunningLeft[2] = LoadTexture("assets/mulher-invisivel/correndo-3 (1).png");
    data->playerRunningLeft[3] = LoadTexture("assets/mulher-invisivel/correndo-4 (1).png");
    data->playerJumping        = LoadTexture("assets/mulher-invisivel/pulando-1.png");
    data->playerJumpingLeft    = LoadTexture("assets/mulher-invisivel/pulando-1-espelhado.png");
    data->playerAttacking      = LoadTexture("assets/mulher-invisivel/atacando.png");
    data->playerAttackingLeft  = LoadTexture("assets/mulher-invisivel/atacando (1).png");
    data->projectileTexture    = LoadTexture("assets/mulher-invisivel/bolha.png");

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
    data->invisible = false;
    data->invisibleTimer = 0;
    data->invisibleUses = 2;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        data->projectiles[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 800 + i * 800, GROUND_Y - 110 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.2f + (float)(rand() % 50) / 100.0f;
        data->enemies[i].width = 90;
        data->enemies[i].height = 110;
        data->enemies[i].state = ENEMY_WALKING;
        data->enemies[i].animFrame = 0;
        data->enemies[i].animTimer = 0;
        data->enemies[i].stateTimer = 0;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 600 + i * 550 + (float)(rand() % 100);
        float size = 60 + (float)(rand() % 35);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
        data->obstacles[i].textureIndex = 0;
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

    if (IsKeyPressed(KEY_Q) && !data->invisible && data->invisibleUses > 0) {
        data->invisible = true;
        data->invisibleTimer = 2.0f;
        data->invisibleUses--;
    }

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
            if (data->currentAnimation != ANIM_IDLE) {
                data->currentAnimation = ANIM_IDLE;
                data->animFrame = 0;
                data->animTimer = 0;
            }
        }
    }

    float animSpeed =
        (data->currentAnimation == ANIM_RUNNING)   ? 0.08f :
        (data->currentAnimation == ANIM_ATTACKING) ? 0.15f :
        (data->currentAnimation == ANIM_JUMPING)   ? 0.12f : 0.1f;

    if (data->animTimer >= animSpeed) {
        data->animTimer = 0;
        data->animFrame++;
        int maxFrames = (data->currentAnimation == ANIM_RUNNING) ? 4 : 1;
        if (data->animFrame >= maxFrames) data->animFrame = 0;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            data->projectiles[i].position.x += data->projectiles[i].speed;

            data->projectiles[i].animTimer += GetFrameTime();
            if (data->projectiles[i].animTimer >= 0.1f) {
                data->projectiles[i].animTimer = 0;
                data->projectiles[i].animFrame = (data->projectiles[i].animFrame + 1) % 4;
            }
            
            float distanceTraveled = fabs(data->projectiles[i].position.x - data->projectiles[i].startPosition.x);
            if (distanceTraveled >= PROJECTILE_MAX_DISTANCE) {
                data->projectiles[i].active = false;
            }

            if (data->projectiles[i].position.x > data->cameraX + W + 50 ||
                data->projectiles[i].position.x < data->cameraX - 50) {
                data->projectiles[i].active = false;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!data->enemies[i].active) {
            if (data->player.position.x + ENEMY_SPAWN_DISTANCE >= data->enemies[i].position.x) {
                data->enemies[i].active = true;
            }
        }

        if (data->enemies[i].active) {
            data->enemies[i].position.x -= data->enemies[i].speed;

            data->enemies[i].animTimer += GetFrameTime();
            if (data->enemies[i].animTimer >= 0.15f) {
                data->enemies[i].animTimer = 0;
                if (data->enemies[i].state == ENEMY_WALKING) {
                    data->enemies[i].animFrame = (data->enemies[i].animFrame + 1) % 2;
                }
            }

            if (data->enemies[i].state == ENEMY_ATTACKING) {
                data->enemies[i].stateTimer -= GetFrameTime();
                if (data->enemies[i].stateTimer <= 0) {
                    data->enemies[i].state = ENEMY_WALKING;
                }
            }

            if (data->enemies[i].position.x < data->cameraX - 100) {
                data->enemies[i].active = false;
            }
        }
    }

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
                data->projectiles[j].position.x - 20,
                data->projectiles[j].position.y - 20,
                40,
                40
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
        if (data->enemies[i].active && !data->invisible) {
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
                if (data->player.position.y + data->player.height < data->enemies[i].position.y + 20) {
                    continue;
                }

                if (data->player.invulnerabilityTimer <= 0) {
                    data->player.health--;
                    data->player.invulnerabilityTimer = 1.5f;
                    data->enemies[i].state = ENEMY_ATTACKING;
                    data->enemies[i].stateTimer = 0.3f;

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

static void DrawLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;

    ClearBackground((Color){5, 10, 20, 255});
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

    Rectangle source = { 0, 0, (float)currentTexture.width, (float)currentTexture.height };
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
            Texture2D currentEnemyTexture;

            switch (data->enemies[i].state) {
                case ENEMY_WALKING:
                    currentEnemyTexture = data->enemyWalking[data->enemies[i].animFrame % 2];
                    break;
                case ENEMY_ATTACKING:
                    currentEnemyTexture = data->enemyAttacking;
                    break;
                default:
                    currentEnemyTexture = data->enemyWalking[0];
                    break;
            }

            Rectangle src = {0, 0, (float)currentEnemyTexture.width, (float)currentEnemyTexture.height};
            Rectangle dst = {
                data->enemies[i].position.x - data->cameraX,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };

            DrawTexturePro(currentEnemyTexture, src, dst, (Vector2){0, 0}, 0, WHITE);
        }
    }

    CommonDrawHealthHearts(data->player.health, data->heartTexture);
    CommonDrawHUD(data->score, data->player.position.x, MAP_LENGTH);

    if (data->invisible) {
        DrawText("INVISIVEL!", W - 180, 20, 20, VIOLET);
    } else {
        DrawText(TextFormat("Q: Invisivel (%d)", data->invisibleUses),
                 W - 180, 20, 18,
                 data->invisibleUses > 0 ? LIGHTGRAY : DARKGRAY);
    }

    DrawText("WASD: Mover | W: Pular | E: Atirar | Q: Invisivel", 10, H - 30, 16, LIGHTGRAY);

    if (data->gameWon)      CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;

    UnloadTexture(data->backgroundTexture);
    UnloadTexture(data->platformTexture);
    UnloadTexture(data->obstacleTexture);
    UnloadTexture(data->heartTexture);

    UnloadTexture(data->enemyWalking[0]);
    UnloadTexture(data->enemyWalking[1]);
    UnloadTexture(data->enemyAttacking);

    UnloadTexture(data->playerIdle);
    UnloadTexture(data->playerIdleLeft);

    for (int i = 0; i < 4; i++) {
        UnloadTexture(data->playerRunning[i]);
        UnloadTexture(data->playerRunningLeft[i]);
    }

    UnloadTexture(data->playerJumping);
    UnloadTexture(data->playerJumpingLeft);
    UnloadTexture(data->playerAttacking);
    UnloadTexture(data->playerAttackingLeft);
    UnloadTexture(data->projectileTexture);

    free(data);
}

Level* CreateLevelMulherInvisivel(void) {
    Level *level = (Level*)malloc(sizeof(Level));
    level->type = LEVEL_MULHER_INVISIVEL;
    level->functions.init   = InitLevelInvisivel;
    level->functions.update = UpdateLevelInvisivel;
    level->functions.draw   = DrawLevelInvisivel;
    level->functions.unload = UnloadLevelInvisivel;
    level->data = NULL;
    level->functions.init(level);
    return level;
}
