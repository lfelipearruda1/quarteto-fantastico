#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define W 960
#define H 540
#define MAX_PROJECTILES 7
#define MAX_ENEMIES 14
#define MAX_OBSTACLES 18
#define GROUND_Y 400
#define MAP_LENGTH 9000
#define ENEMY_SPAWN_DISTANCE 650

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

typedef struct {
    Vector2 position;
    Vector2 velocity;
    bool onGround;
    int health;
    float width;
    float height;
    bool invisible;
    float invisibleTimer;
} GameCharacter;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
} Projectile;

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
    Projectile projectiles[MAX_PROJECTILES];
    Enemy enemies[MAX_ENEMIES];
    Obstacle obstacles[MAX_OBSTACLES];
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
    data->player.invisible = false;
    data->player.invisibleTimer = 0;

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

    if (data->player.invisible) {
        data->player.invisibleTimer -= GetFrameTime();
        if (data->player.invisibleTimer <= 0) {
            data->player.invisible = false;
        }
    }

    data->player.velocity.x = 0;

    if (IsKeyDown(KEY_A)) {
        data->player.velocity.x = -5.0f;
    }
    if (IsKeyDown(KEY_D)) {
        data->player.velocity.x = 5.0f;
    }

    if (IsKeyPressed(KEY_Q) && !data->player.invisible) {
        data->player.invisible = true;
        data->player.invisibleTimer = 2.0f;
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
    }

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        ShootProjectile(data);
    }

    data->player.velocity.y += 0.6f;
    data->player.position.y += data->player.velocity.y;

    data->player.onGround = false;

    if (data->player.position.y + data->player.height >= GROUND_Y) {
        data->player.position.y = GROUND_Y - data->player.height;
        data->player.velocity.y = 0;
        data->player.onGround = true;
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

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (data->projectiles[i].active) {
            data->projectiles[i].position.x += data->projectiles[i].speed;

            if (data->projectiles[i].position.x > data->cameraX + W + 50) {
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

            if (!data->player.invisible && CheckCollisionRecs(playerRect, enemyRect)) {
                data->player.health--;
                data->enemies[i].active = false;
                if (data->player.health <= 0) {
                    data->gameLost = true;
                }
            }

            for (int j = 0; j < MAX_PROJECTILES; j++) {
                if (data->projectiles[j].active) {
                    Rectangle projRect = {
                        data->projectiles[j].position.x - 15,
                        data->projectiles[j].position.y - 15,
                        30,
                        30
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

    if (data->player.position.x >= MAP_LENGTH) {
        data->gameWon = true;
        data->score += 500;
        data->score += data->player.health * 100;
    }

    if (data->player.position.y > H) {
        data->gameLost = true;
    }
}

static void DrawLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;

    ClearBackground((Color){5, 10, 20, 255});

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

    Rectangle source = {0, 0, (float)data->playerTexture.width, (float)data->playerTexture.height};
    Rectangle dest = {
        data->player.position.x - data->cameraX,
        data->player.position.y,
        data->player.width,
        data->player.height
    };
    
    if (data->player.invisible) {
        DrawTexturePro(data->playerTexture, source, dest, (Vector2){0, 0}, 0, Fade(WHITE, 0.3f));
    } else {
        DrawTexturePro(data->playerTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }

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

    for (int i = 0; i < data->player.health && i < 3; i++) {
        Rectangle source = {0, 0, (float)data->heartTexture.width, (float)data->heartTexture.height};
        Rectangle dest = {10 + i * 55, 10, 40, 40};
        DrawTexturePro(data->heartTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }

    DrawText(TextFormat("PONTOS: %d", data->score), 180, 20, 22, GOLD);
    DrawText(TextFormat("DISTANCIA: %.0f/%.0f", data->player.position.x, (float)MAP_LENGTH), 450, 20, 20, WHITE);
    
    if (data->player.invisible) {
        DrawText("INVISIVEL!", W - 150, 20, 20, VIOLET);
    } else {
        DrawText("Q: Invisivel", W - 150, 20, 18, GRAY);
    }

    float progress = data->player.position.x / MAP_LENGTH;
    DrawRectangle(10, H - 30, (int)(W - 20), 20, DARKGRAY);
    DrawRectangle(10, H - 30, (int)((W - 20) * progress), 20, VIOLET);

    DrawText("WASD: Mover | W: Pular | E: Atirar | Q: Invisivel", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) {
        DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
        const char *winText = "VITORIA!";
        int textWidth = MeasureText(winText, 60);
        DrawText(winText, W/2 - textWidth/2, H/2 - 60, 60, VIOLET);

        const char *scoreText = TextFormat("Pontuacao Final: %d", data->score);
        int scoreWidth = MeasureText(scoreText, 30);
        DrawText(scoreText, W/2 - scoreWidth/2, H/2 + 20, 30, GOLD);

        const char *continueText = "Pressione ENTER para continuar...";
        int continueWidth = MeasureText(continueText, 20);
        DrawText(continueText, W/2 - continueWidth/2, H/2 + 80, 20, WHITE);
    } else if (data->gameLost) {
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

static void UnloadLevelInvisivel(Level *level) {
    LevelInvisivelData *data = (LevelInvisivelData*)level->data;
    if (data->backgroundTexture.id > 0) {
        UnloadTexture(data->backgroundTexture);
    }
    if (data->playerTexture.id > 0) {
        UnloadTexture(data->playerTexture);
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

