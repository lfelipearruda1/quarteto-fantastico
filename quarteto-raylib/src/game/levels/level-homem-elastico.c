#include "level-common.h"
#include <stdlib.h>
#include <math.h>

#define MAX_PROJECTILES 5
#define MAX_ENEMIES 20
#define MAX_OBSTACLES 28
#define MAP_LENGTH 12000
#define ENEMY_SPAWN_DISTANCE 500
#define PROJECTILE_MAX_DISTANCE 150

typedef enum {
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_JUMPING,
    ANIM_ATTACKING
} AnimationState;

typedef enum {
    ENEMY_WALKING,
    ENEMY_ATTACKING
} EnemyState;

typedef struct {
    Vector2 position;
    Vector2 startPosition;
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
    int punchesRemaining;

    Projectile projectiles[MAX_PROJECTILES];
    BaseObstacle obstacles[MAX_OBSTACLES];

    struct {
        Vector2 position;
        bool active;
        float speed;
        float width;
        float height;

        EnemyState state;
        int animFrame;
        float animTimer;
        float stateTimer;
    } enemies[MAX_ENEMIES];

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

    Texture2D enemyWalking[2];
    Texture2D enemyAttacking;
} LevelElasticoData;

static void ShootPunch(LevelElasticoData *data)
{
    if (data->punchesRemaining <= 0) return;

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!data->projectiles[i].active)
        {
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

            data->projectiles[i].startPosition = data->projectiles[i].position;
            data->projectiles[i].active = true;

            data->isAttacking = true;
            data->attackTimer = 0.3f;
            data->currentAnimation = ANIM_ATTACKING;
            data->animFrame = 0;

            data->punchesRemaining--;
            break;
        }
    }
}

static void InitLevelElastico(Level *level)
{
    LevelElasticoData *data = malloc(sizeof(LevelElasticoData));
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

    data->enemyWalking[0] = LoadTexture("assets/vilao/andando-1-espelhado.png");
    data->enemyWalking[1] = LoadTexture("assets/vilao/andando-2-espelhado.png");
    data->enemyAttacking  = LoadTexture("assets/vilao/luta-espelhado.png");

    data->playerIdle = LoadTexture("assets/homem-elastico/correndo-1.png");
    data->playerIdleLeft = LoadTexture("assets/homem-elastico/correndo-1 (1).png");

    for (int i = 0; i < 4; i++)
    {
        data->playerRunning[i] = LoadTexture(TextFormat("assets/homem-elastico/correndo-%d.png", i+1));
        data->playerRunningLeft[i] = LoadTexture(TextFormat("assets/homem-elastico/correndo-%d (1).png", i+1));
    }

    data->playerJumping = LoadTexture("assets/homem-elastico/pulando-1.png");
    data->playerJumpingLeft = LoadTexture("assets/homem-elastico/pulando-1 (1).png");

    data->playerAttacking = LoadTexture("assets/homem-elastico/atacando-1.png");
    data->playerAttackingLeft = LoadTexture("assets/homem-elastico/atacando-1 (1).png");

    data->player.position = (Vector2){100, GROUND_Y - 110};
    data->player.velocity = (Vector2){0, 0};
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
    data->punchesRemaining = 5;

    for (int i = 0; i < MAX_PROJECTILES; i++)
        data->projectiles[i].active = false;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        data->enemies[i].position = (Vector2){600 + i * 500, GROUND_Y - 110};
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.4f + (rand() % 100) / 80.0f;
        data->enemies[i].width = 90;
        data->enemies[i].height = 110;

        data->enemies[i].state = ENEMY_WALKING;
        data->enemies[i].animFrame = 0;
        data->enemies[i].animTimer = 0;
        data->enemies[i].stateTimer = 0;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        float x = 400 + i * 420 + (rand() % 150);
        float size = 70 + (rand() % 50);
        data->obstacles[i].rect = (Rectangle){x, GROUND_Y - size, size, size};
        data->obstacles[i].active = true;
        data->obstacles[i].textureIndex = 0;
    }
}

static void UpdateLevelElastico(Level *level, GameState *state)
{
    LevelElasticoData *data = level->data;

    CommonHandleGameEnd(data->gameWon, data->gameLost, &data->gameEndTimer, data->score, state);
    if (data->gameWon || data->gameLost) return;

    if (data->isAttacking)
    {
        data->attackTimer -= GetFrameTime();
        if (data->attackTimer <= 0)
            data->isAttacking = false;
    }

    CommonUpdateInvulnerability(&data->player);
    data->animTimer += GetFrameTime();

    CommonUpdateHorizontalMovement(&data->player);
    CommonClampPlayerToCamera(&data->player, data->cameraX);
    CommonUpdateCamera(&data->cameraX, &data->player);
    CommonHandleJump(&data->player);

    if (IsKeyPressed(KEY_E))
        ShootPunch(data);

    CommonApplyGravity(&data->player);
    CommonCheckGroundCollision(&data->player, GROUND_Y);
    CommonCheckObstacleCollision(&data->player, data->obstacles, MAX_OBSTACLES);

    if (!data->isAttacking)
    {
        if (!data->player.onGround)
            data->currentAnimation = ANIM_JUMPING;
        else if (data->player.velocity.x != 0)
            data->currentAnimation = ANIM_RUNNING;
        else
            data->currentAnimation = ANIM_IDLE;
    }

    float animSpeed =
        data->currentAnimation == ANIM_RUNNING ? 0.08f :
        data->currentAnimation == ANIM_JUMPING ? 0.15f :
        data->currentAnimation == ANIM_ATTACKING ? 0.15f : 0.1f;

    if (data->animTimer >= animSpeed)
    {
        data->animTimer = 0;
        data->animFrame++;
        if (data->currentAnimation == ANIM_RUNNING && data->animFrame >= 4)
            data->animFrame = 0;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!data->projectiles[i].active) continue;

        data->projectiles[i].position.x += data->projectiles[i].speed;

        float dist = fabs(data->projectiles[i].position.x - data->projectiles[i].startPosition.x);
        if (dist >= PROJECTILE_MAX_DISTANCE)
            data->projectiles[i].active = false;

        if (data->projectiles[i].position.x > data->cameraX + W + 100 ||
            data->projectiles[i].position.x < data->cameraX - 100)
            data->projectiles[i].active = false;
    }

    CommonActivateEnemiesNearPlayer((BaseEnemy*)data->enemies, MAX_ENEMIES, data->player.position.x, ENEMY_SPAWN_DISTANCE);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!data->enemies[i].active) continue;

        data->enemies[i].position.x -= data->enemies[i].speed;

        data->enemies[i].animTimer += GetFrameTime();
        if (data->enemies[i].animTimer >= 0.15f)
        {
            data->enemies[i].animTimer = 0;

            if (data->enemies[i].state == ENEMY_WALKING)
                data->enemies[i].animFrame = (data->enemies[i].animFrame + 1) % 2;
        }

        if (data->enemies[i].state == ENEMY_ATTACKING)
        {
            data->enemies[i].stateTimer -= GetFrameTime();
            if (data->enemies[i].stateTimer <= 0)
                data->enemies[i].state = ENEMY_WALKING;
        }

        if (data->enemies[i].position.x < data->cameraX - 120)
            data->enemies[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!data->enemies[i].active) continue;

        Rectangle enemyBox = {
            data->enemies[i].position.x,
            data->enemies[i].position.y,
            data->enemies[i].width,
            data->enemies[i].height
        };

        for (int j = 0; j < MAX_PROJECTILES; j++)
        {
            if (!data->projectiles[j].active) continue;

            Rectangle projBox = {
                data->projectiles[j].position.x - 20,
                data->projectiles[j].position.y - 20,
                40, 40
            };

            if (CheckCollisionRecs(enemyBox, projBox))
            {
                data->enemies[i].active = false;
                data->enemies[i].position.x = -9999;
                data->projectiles[j].active = false;
                data->score += 100;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!data->enemies[i].active) continue;

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

        if (CheckCollisionRecs(playerRect, enemyRect))
        {
            if (data->player.invulnerabilityTimer <= 0)
            {
                data->player.health--;
                data->player.invulnerabilityTimer = 1.5f;

                // inimigo entra no estado ataque
                data->enemies[i].state = ENEMY_ATTACKING;
                data->enemies[i].stateTimer = 0.3f;

                if (data->player.health <= 0)
                    data->gameLost = true;
            }
        }
    }

    if (CommonCheckVictory(data->player.position.x, MAP_LENGTH))
    {
        data->gameWon = true;
        data->score += 500 + data->player.health * 100;
    }

    if (CommonCheckFall(data->player.position.y))
        data->gameLost = true;
}

static void DrawLevelElastico(Level *level)
{
    LevelElasticoData *data = level->data;

    ClearBackground((Color){10, 20, 40, 255});
    CommonDrawBackground(data->backgroundTexture, data->cameraX);
    CommonDrawPlatform(data->platformTexture, data->cameraX, MAP_LENGTH);
    CommonDrawObstacles(data->obstacles, MAX_OBSTACLES, &data->obstacleTexture, 1, data->cameraX);

    Texture2D currentTexture;

    if (data->player.facingRight)
    {
        switch (data->currentAnimation)
        {
            case ANIM_RUNNING:   currentTexture = data->playerRunning[data->animFrame % 4]; break;
            case ANIM_JUMPING:   currentTexture = data->playerJumping; break;
            case ANIM_ATTACKING: currentTexture = data->playerAttacking; break;
            default:             currentTexture = data->playerIdle;
        }
    }
    else
    {
        switch (data->currentAnimation)
        {
            case ANIM_RUNNING:   currentTexture = data->playerRunningLeft[data->animFrame % 4]; break;
            case ANIM_JUMPING:   currentTexture = data->playerJumpingLeft; break;
            case ANIM_ATTACKING: currentTexture = data->playerAttackingLeft; break;
            default:             currentTexture = data->playerIdleLeft;
        }
    }

    Rectangle src = {0, 0, currentTexture.width, currentTexture.height};
    Rectangle dst = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };

    DrawTexturePro(currentTexture, src, dst, (Vector2){0, 0}, 0, CommonGetPlayerColor(&data->player));

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (!data->projectiles[i].active) continue;

        DrawCircle(
            data->projectiles[i].position.x - data->cameraX,
            data->projectiles[i].position.y,
            20, SKYBLUE
        );
        DrawCircle(
            data->projectiles[i].position.x - data->cameraX,
            data->projectiles[i].position.y,
            15, BLUE
        );
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!data->enemies[i].active) continue;

        Texture2D enemyTex;

        if (data->enemies[i].state == ENEMY_WALKING)
            enemyTex = data->enemyWalking[data->enemies[i].animFrame % 2];
        else
            enemyTex = data->enemyAttacking;

        Rectangle esrc = {0, 0, enemyTex.width, enemyTex.height};
        Rectangle edst = {
            data->enemies[i].position.x - data->cameraX,
            data->enemies[i].position.y,
            data->enemies[i].width,
            data->enemies[i].height
        };

        DrawTexturePro(enemyTex, esrc, edst, (Vector2){0, 0}, 0, WHITE);
    }

    CommonDrawHealthHearts(data->player.health, data->heartTexture);
    CommonDrawHUD(data->score, data->player.position.x, MAP_LENGTH);

    DrawText(TextFormat("Socos (E): %d", data->punchesRemaining),
             W - 200, 20, 20, SKYBLUE);

    DrawText("WASD - mover | Espaço/W - pular | E - soco-energia",
             10, H - 30, 16, WHITE);

    if (data->gameWon) CommonDrawVictoryScreen(data->score);
    else if (data->gameLost) CommonDrawGameOverScreen(data->score);
}

static void UnloadLevelElastico(Level *level)
{
    LevelElasticoData *data = level->data;

    UnloadTexture(data->backgroundTexture);
    UnloadTexture(data->platformTexture);
    UnloadTexture(data->obstacleTexture);
    UnloadTexture(data->heartTexture);

    UnloadTexture(data->enemyWalking[0]);
    UnloadTexture(data->enemyWalking[1]);
    UnloadTexture(data->enemyAttacking);

    UnloadTexture(data->playerIdle);
    UnloadTexture(data->playerIdleLeft);

    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(data->playerRunning[i]);
        UnloadTexture(data->playerRunningLeft[i]);
    }

    UnloadTexture(data->playerJumping);
    UnloadTexture(data->playerJumpingLeft);
    UnloadTexture(data->playerAttacking);
    UnloadTexture(data->playerAttackingLeft);
}

Level* CreateLevelHomemElastico(void)
{
    Level *level = malloc(sizeof(Level));
    level->type = LEVEL_HOMEM_ELASTICO;
    level->functions.init = InitLevelElastico;
    level->functions.update = UpdateLevelElastico;
    level->functions.draw = DrawLevelElastico;
    level->functions.unload = UnloadLevelElastico;
    level->data = NULL;
    level->functions.init(level);
    return level;
}
