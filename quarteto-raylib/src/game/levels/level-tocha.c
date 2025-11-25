#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define W 960
#define H 540
#define MAX_FIREBALLS 10
#define MAX_ENEMIES 10
#define MAX_OBSTACLES 12
#define GROUND_Y 400
#define MAP_LENGTH 6000
#define ENEMY_SPAWN_DISTANCE 500

typedef enum {
    STATE_LOGO,
    STATE_SELECT,
    STATE_RANKING,
    STATE_GAME
} GameState;

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

typedef struct Player {
    char *name;
    int levelChosen;
    int score;
    struct Player *next;
} Player;

extern Player *playerList;
void UpdatePlayerScore(const char *name, int newScore);

typedef enum {
    ANIM_IDLE,
    ANIM_RUNNING,
    ANIM_JUMPING,
    ANIM_ATTACKING
} AnimationState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool onGround;
    int health;
    float width;
    float height;
    AnimationState currentAnimation;
    int animFrame;
    float animTimer;
    float attackTimer;
    bool isAttacking;
} GameCharacter;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    int animFrame;
    float animTimer;
} Fireball;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float width;
    float height;
} Enemy;

typedef struct {
    Rectangle rect;
    bool active;
} Obstacle;

typedef struct {
    GameCharacter player;
    Fireball fireballs[MAX_FIREBALLS];
    Enemy enemies[MAX_ENEMIES];
    Obstacle obstacles[MAX_OBSTACLES];
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
    Texture2D playerRunning[3];
    Texture2D playerJumping[3];
    Texture2D playerAttacking[2];

    Texture2D fireballTexture;

    Texture2D enemyTexture;
} LevelTochaData;

static void ShootFireball(LevelTochaData *data) {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!data->fireballs[i].active) {
            data->fireballs[i].position = (Vector2){
                data->player.position.x + data->player.width,
                data->player.position.y + data->player.height / 2
            };
            data->fireballs[i].active = true;
            data->fireballs[i].speed = 12.0f;
            data->fireballs[i].animFrame = 0;
            data->fireballs[i].animTimer = 0;

            data->player.isAttacking = true;
            data->player.attackTimer = 0.3f;
            data->player.currentAnimation = ANIM_ATTACKING;
            data->player.animFrame = 0;
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

    data->backgroundTexture = LoadTexture("assets/coisa/fundoicoisa.png");
    data->platformTexture = LoadTexture("assets/coisa/plataforma-coisa.png");
    data->obstacleTexture = LoadTexture("assets/coisa/obstaculo-coisa.png");
    data->heartTexture = LoadTexture("assets/coracao.png");
    data->enemyTexture = LoadTexture("assets/coisa/inimigo-coisa.png");

    data->playerIdle = LoadTexture("assets/tocha-humana/parado-1-Photoroom.png");
    data->playerRunning[0] = LoadTexture("assets/tocha-humana/correndo-1.png");
    data->playerRunning[1] = LoadTexture("assets/tocha-humana/correndo-2.png");
    data->playerRunning[2] = LoadTexture("assets/tocha-humana/correndo-3.png");
    data->playerJumping[0] = LoadTexture("assets/tocha-humana/pulando-1-Photoroom.png");
    data->playerJumping[1] = LoadTexture("assets/tocha-humana/pulando-2-Photoroom.png");
    data->playerJumping[2] = LoadTexture("assets/tocha-humana/pulando-3-Photoroom.png");
    data->playerAttacking[0] = LoadTexture("assets/tocha-humana/atacando-1.png");
    data->playerAttacking[1] = LoadTexture("assets/tocha-humana/atacando-2-Photoroom.png");

    data->fireballTexture = LoadTexture("assets/tocha-humana/bola-de-fogo-Photoroom.png");

    data->player.position = (Vector2){ 100, GROUND_Y - 110 };
    data->player.velocity = (Vector2){ 0, 0 };
    data->player.onGround = true;
    data->player.health = 3;
    data->player.width = 90;
    data->player.height = 110;
    data->player.currentAnimation = ANIM_IDLE;
    data->player.animFrame = 0;
    data->player.animTimer = 0;
    data->player.attackTimer = 0;
    data->player.isAttacking = false;

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        data->fireballs[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        data->enemies[i].position = (Vector2){ 800 + i * 600, GROUND_Y - 70 };
        data->enemies[i].active = false;
        data->enemies[i].speed = 1.2f + (float)(rand() % 60) / 100.0f;
        data->enemies[i].width = 70;
        data->enemies[i].height = 70;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        float x = 600 + i * 500 + (float)(rand() % 100);
        float size = 40 + (float)(rand() % 25);
        data->obstacles[i].rect = (Rectangle){ x, GROUND_Y - size, size, size };
        data->obstacles[i].active = true;
    }
}

static void UpdateLevelTocha(Level *level, GameState *state) {
    LevelTochaData *data = (LevelTochaData*)level->data;

    if (data->gameWon || data->gameLost) {
        data->gameEndTimer += GetFrameTime();

        if (IsKeyPressed(KEY_ENTER) || data->gameEndTimer > 3.0f) {
            if (data->gameWon) {
                if (playerList != NULL) {
                    playerList->score += data->score;
                    UpdatePlayerScore(playerList->name, playerList->score);
                }
            }
            *state = STATE_SELECT;
            return;
        }
        return;
    }

    if (data->player.isAttacking) {
        data->player.attackTimer -= GetFrameTime();
        if (data->player.attackTimer <= 0) {
            data->player.isAttacking = false;
        }
    }

    data->player.animTimer += GetFrameTime();

    data->player.velocity.x = 0;

    if (IsKeyDown(KEY_A)) {
        data->player.velocity.x = -5.0f;
    }
    if (IsKeyDown(KEY_D)) {
        data->player.velocity.x = 5.0f;
    }

    data->player.position.x += data->player.velocity.x;

    if (data->player.position.x < data->cameraX) {
        data->player.position.x = data->cameraX;
    }
    if (data->player.position.x + data->player.width > data->cameraX + W) {
        data->player.position.x = data->cameraX + W - data->player.width;
    }

    data->cameraX = data->player.position.x - 200;
    if (data->cameraX < 0) data->cameraX = 0;

    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && data->player.onGround) {
        data->player.velocity.y = -15.0f;
        data->player.onGround = false;
        data->player.currentAnimation = ANIM_JUMPING;
        data->player.animFrame = 0;
    }

    if (IsKeyPressed(KEY_E)) {
        ShootFireball(data);
    }

    data->player.velocity.y += 0.6f;
    data->player.position.y += data->player.velocity.y;

    data->player.onGround = false;

    if (data->player.position.y + data->player.height >= GROUND_Y) {
        data->player.position.y = GROUND_Y - data->player.height;
        data->player.velocity.y = 0;
        data->player.onGround = true;
    }

    if (!data->player.isAttacking) {
        if (!data->player.onGround) {
            if (data->player.currentAnimation != ANIM_JUMPING) {
                data->player.currentAnimation = ANIM_JUMPING;
                data->player.animFrame = 0;
                data->player.animTimer = 0;
            }
        } else if (data->player.velocity.x != 0) {
            if (data->player.currentAnimation != ANIM_RUNNING) {
                data->player.currentAnimation = ANIM_RUNNING;
                data->player.animFrame = 0;
                data->player.animTimer = 0;
            }
        } else {
            data->player.currentAnimation = ANIM_IDLE;
            data->player.animFrame = 0;
        }
    }

    float animSpeed = 0.1f;
    if (data->player.currentAnimation == ANIM_RUNNING) {
        animSpeed = 0.08f;
    } else if (data->player.currentAnimation == ANIM_JUMPING) {
        animSpeed = 0.15f;
    } else if (data->player.currentAnimation == ANIM_ATTACKING) {
        animSpeed = 0.15f;
    }

    if (data->player.animTimer >= animSpeed) {
        data->player.animTimer = 0;
        data->player.animFrame++;

        int maxFrames = 1;
        if (data->player.currentAnimation == ANIM_RUNNING) {
            maxFrames = 3;
        } else if (data->player.currentAnimation == ANIM_JUMPING) {
            maxFrames = 3;
        } else if (data->player.currentAnimation == ANIM_ATTACKING) {
            maxFrames = 2;
        }

        if (data->player.animFrame >= maxFrames) {
            data->player.animFrame = 0;
        }
    }

    Rectangle playerRect = {
        data->player.position.x,
        data->player.position.y,
        data->player.width,
        data->player.height
    };

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (data->obstacles[i].active) {
            if (CheckCollisionRecs(playerRect, data->obstacles[i].rect)) {
                float playerBottom = data->player.position.y + data->player.height;
                float playerTop = data->player.position.y;
                float obstacleTop = data->obstacles[i].rect.y;
                float obstacleBottom = data->obstacles[i].rect.y + data->obstacles[i].rect.height;

                if (data->player.velocity.y >= 0 && playerBottom - data->player.velocity.y <= obstacleTop + 5) {
                    data->player.position.y = obstacleTop - data->player.height;
                    data->player.velocity.y = 0;
                    data->player.onGround = true;
                } else if (data->player.velocity.y < 0 && playerTop - data->player.velocity.y >= obstacleBottom) {
                    data->player.position.y = obstacleBottom;
                    data->player.velocity.y = 0;
                } else if (data->player.velocity.x > 0) {
                    data->player.position.x = data->obstacles[i].rect.x - data->player.width;
                } else if (data->player.velocity.x < 0) {
                    data->player.position.x = data->obstacles[i].rect.x + data->obstacles[i].rect.width;
                }
            }
        }
    }

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (data->fireballs[i].active) {
            data->fireballs[i].position.x += data->fireballs[i].speed;

            data->fireballs[i].animTimer += GetFrameTime();
            if (data->fireballs[i].animTimer >= 0.1f) {
                data->fireballs[i].animTimer = 0;
                data->fireballs[i].animFrame = (data->fireballs[i].animFrame + 1) % 4;
            }

            if (data->fireballs[i].position.x > data->cameraX + W + 50) {
                data->fireballs[i].active = false;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!data->enemies[i].active) {
            if (data->player.position.x + ENEMY_SPAWN_DISTANCE >= data->enemies[i].position.x) {
                data->enemies[i].active = true;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            data->enemies[i].position.x -= data->enemies[i].speed;

            if (data->enemies[i].position.x < data->cameraX - 100) {
                data->enemies[i].active = false;
            }

            Rectangle enemyRect = {
                data->enemies[i].position.x,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };

            if (CheckCollisionRecs(playerRect, enemyRect)) {
                data->player.health--;
                data->enemies[i].active = false;
                if (data->player.health <= 0) {
                    data->gameLost = true;
                }
            }

            for (int j = 0; j < MAX_FIREBALLS; j++) {
                if (data->fireballs[j].active) {
                    Rectangle fireballRect = {
                        data->fireballs[j].position.x - 20,
                        data->fireballs[j].position.y - 20,
                        40,
                        40
                    };

                    if (CheckCollisionRecs(fireballRect, enemyRect)) {
                        data->enemies[i].active = false;
                        data->fireballs[j].active = false;
                        data->score += 100;
                    }
                }
            }
        }
    }

    if (data->player.position.x >= MAP_LENGTH) {
        data->gameWon = true;
        data->score += 500;
        data->score += data->player.health * 100;
    }

    if (data->player.position.y > H) {
        data->gameLost = true;
    }
}

static void DrawLevelTocha(Level *level) {
    LevelTochaData *data = (LevelTochaData*)level->data;

    ClearBackground((Color){30, 15, 50, 255});

    float parallaxSpeed = 0.5f;
    float bgX = -(data->cameraX * parallaxSpeed);

    int numCopies = (int)((data->cameraX * parallaxSpeed + W) / data->backgroundTexture.width) + 2;
    for (int i = -1; i < numCopies; i++) {
        float drawX = bgX + (i * data->backgroundTexture.width);
        Rectangle source = {0, 0, (float)data->backgroundTexture.width, (float)data->backgroundTexture.height};
        Rectangle dest = {drawX, 0, (float)data->backgroundTexture.width, (float)H};
        DrawTexturePro(data->backgroundTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }

    int tileWidth = data->platformTexture.width;
    int tileHeight = H - GROUND_Y;
    int startTile = (int)(data->cameraX / tileWidth);
    int endTile = (int)((data->cameraX + W) / tileWidth) + 2;

    for (int i = startTile; i < endTile && i * tileWidth < MAP_LENGTH + 1000; i++) {
        Rectangle source = {0, 0, (float)data->platformTexture.width, (float)data->platformTexture.height};
        Rectangle dest = {
            i * tileWidth - data->cameraX,
            GROUND_Y,
            (float)tileWidth,
            (float)tileHeight
        };
        DrawTexturePro(data->platformTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (data->obstacles[i].active) {
            Rectangle drawRect = data->obstacles[i].rect;
            drawRect.x -= data->cameraX;

            Rectangle source = {0, 0, (float)data->obstacleTexture.width, (float)data->obstacleTexture.height};
            DrawTexturePro(data->obstacleTexture, source, drawRect, (Vector2){0, 0}, 0, WHITE);
        }
    }

    Texture2D currentTexture = data->playerIdle;

    switch (data->player.currentAnimation) {
        case ANIM_IDLE:
            currentTexture = data->playerIdle;
            break;
        case ANIM_RUNNING:
            currentTexture = data->playerRunning[data->player.animFrame % 3];
            break;
        case ANIM_JUMPING:
            currentTexture = data->playerJumping[data->player.animFrame % 3];
            break;
        case ANIM_ATTACKING:
            currentTexture = data->playerAttacking[data->player.animFrame % 2];
            break;
    }

    Rectangle source = {0, 0, (float)currentTexture.width, (float)currentTexture.height};
    Rectangle dest = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };
    DrawTexturePro(currentTexture, source, dest, (Vector2){0, 0}, 0, WHITE);

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (data->fireballs[i].active) {
            Rectangle source = {0, 0, (float)data->fireballTexture.width, (float)data->fireballTexture.height};
            Rectangle dest = {
                data->fireballs[i].position.x - data->cameraX - 20,
                data->fireballs[i].position.y - 20,
                40,
                40
            };

            float rotation = data->fireballs[i].animFrame * 90.0f;
            Vector2 origin = {20, 20};

            DrawTexturePro(data->fireballTexture, source, dest, origin, rotation, WHITE);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (data->enemies[i].active) {
            Rectangle source = {0, 0, (float)data->enemyTexture.width, (float)data->enemyTexture.height};
            Rectangle dest = {
                data->enemies[i].position.x - data->cameraX,
                data->enemies[i].position.y,
                data->enemies[i].width,
                data->enemies[i].height
            };
            DrawTexturePro(data->enemyTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
        }
    }

    for (int i = 0; i < data->player.health && i < 3; i++) {
        Rectangle source = {0, 0, (float)data->heartTexture.width, (float)data->heartTexture.height};
        Rectangle dest = {10 + i * 55, 10, 40, 40};
        DrawTexturePro(data->heartTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }

    DrawText(TextFormat("PONTOS: %d", data->score), 180, 20, 22, GOLD);
    DrawText(TextFormat("DISTANCIA: %.0f/%.0f", data->player.position.x, (float)MAP_LENGTH), 450, 20, 20, WHITE);

    float progress = data->player.position.x / MAP_LENGTH;
    DrawRectangle(10, H - 30, (int)(W - 20), 20, DARKGRAY);
    DrawRectangle(10, H - 30, (int)((W - 20) * progress), 20, ORANGE);

    DrawText("WASD: Mover | W/ESPACO: Pular | E: Lancar Bola de Fogo", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) {
        DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
        const char *winText = "VITORIA!";
        int textWidth = MeasureText(winText, 60);
        DrawText(winText, W/2 - textWidth/2, H/2 - 60, 60, ORANGE);

        const char *scoreText = TextFormat("Pontuacao Final: %d", data->score);
        int scoreWidth = MeasureText(scoreText, 30);
        DrawText(scoreText, W/2 - scoreWidth/2, H/2 + 20, 30, GOLD);

        const char *continueText = "Pressione ENTER para continuar...";
        int continueWidth = MeasureText(continueText, 20);
        DrawText(continueText, W/2 - continueWidth/2, H/2 + 80, 20, WHITE);
    }
    else if (data->gameLost) {
        DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
        const char *loseText = "GAME OVER!";
        int textWidth = MeasureText(loseText, 60);
        DrawText(loseText, W/2 - textWidth/2, H/2 - 60, 60, RED);

        const char *scoreText = TextFormat("Pontuacao: %d", data->score);
        int scoreWidth = MeasureText(scoreText, 30);
        DrawText(scoreText, W/2 - scoreWidth/2, H/2 + 20, 30, GOLD);

        const char *continueText = "Pressione ENTER para voltar...";
        int continueWidth = MeasureText(continueText, 20);
        DrawText(continueText, W/2 - continueWidth/2, H/2 + 80, 20, WHITE);
    }
}

static void UnloadLevelTocha(Level *level) {
    LevelTochaData *data = (LevelTochaData*)level->data;

    if (data->backgroundTexture.id > 0) {
        UnloadTexture(data->backgroundTexture);
    }
    if (data->platformTexture.id > 0) {
        UnloadTexture(data->platformTexture);
    }
    if (data->obstacleTexture.id > 0) {
        UnloadTexture(data->obstacleTexture);
    }
    if (data->heartTexture.id > 0) {
        UnloadTexture(data->heartTexture);
    }
    if (data->enemyTexture.id > 0) {
        UnloadTexture(data->enemyTexture);
    }

    if (data->playerIdle.id > 0) {
        UnloadTexture(data->playerIdle);
    }
    for (int i = 0; i < 3; i++) {
        if (data->playerRunning[i].id > 0) {
            UnloadTexture(data->playerRunning[i]);
        }
        if (data->playerJumping[i].id > 0) {
            UnloadTexture(data->playerJumping[i]);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (data->playerAttacking[i].id > 0) {
            UnloadTexture(data->playerAttacking[i]);
        }
    }

    if (data->fireballTexture.id > 0) {
        UnloadTexture(data->fireballTexture);
    }
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
