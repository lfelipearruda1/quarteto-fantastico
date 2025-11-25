#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define W 960
#define H 540
#define MAX_PUNCHES 5
#define MAX_ENEMIES 10
#define MAX_OBSTACLES 12
#define GROUND_Y 400
#define MAP_LENGTH 6500
#define ENEMY_SPAWN_DISTANCE 550

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
} GameCharacter;

typedef struct {
    Vector2 position;
    bool active;
    float speed;
    float startX;
} Punch;

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
    Punch punches[MAX_PUNCHES];
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
    data->playerTexture = LoadTexture("assets/homem-elastico/foto-homem-elastico.png");
    data->platformTexture = LoadTexture("assets/coisa/plataforma-coisa.png");
    data->obstacleTexture = LoadTexture("assets/coisa/obstaculo-coisa.png");
    data->heartTexture = LoadTexture("assets/coracao.png");

    data->player.position = (Vector2){ 100, GROUND_Y - 110 };
    data->player.velocity = (Vector2){ 0, 0 };
    data->player.onGround = true;
    data->player.health = 3;
    data->player.width = 90;
    data->player.height = 110;

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
    }

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        ShootPunch(data);
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

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            data->punches[i].position.x += data->punches[i].speed;

            if (data->punches[i].position.x - data->punches[i].startX > 450 ||
                data->punches[i].position.x > data->cameraX + W + 50) {
                data->punches[i].active = false;
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

            for (int j = 0; j < MAX_PUNCHES; j++) {
                if (data->punches[j].active) {
                    Rectangle punchRect = {
                        data->punches[j].position.x - 15,
                        data->punches[j].position.y - 15,
                        30,
                        30
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

    if (data->player.position.x >= MAP_LENGTH) {
        data->gameWon = true;
        data->score += 500;
        data->score += data->player.health * 100;
    }

    if (data->player.position.y > H) {
        data->gameLost = true;
    }
}

static void DrawLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;

    ClearBackground((Color){10, 20, 40, 255});

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
    DrawTexturePro(data->playerTexture, source, dest, (Vector2){0, 0}, 0, WHITE);

    for (int i = 0; i < MAX_PUNCHES; i++) {
        if (data->punches[i].active) {
            float distance = data->punches[i].position.x - data->punches[i].startX;
            float maxDistance = 450;
            float width = 10 + (distance / maxDistance) * 40;
            
            DrawRectangle(
                (int)(data->punches[i].startX - data->cameraX),
                (int)(data->punches[i].position.y - 5),
                (int)width,
                10,
                BLUE
            );
            DrawCircle(
                (int)(data->punches[i].position.x - data->cameraX),
                (int)data->punches[i].position.y,
                15,
                SKYBLUE
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
            DrawRectangleRec(enemyDrawRect, PURPLE);
            DrawRectangleLinesEx(enemyDrawRect, 2, DARKPURPLE);
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
    DrawRectangle(10, H - 30, (int)((W - 20) * progress), 20, BLUE);

    DrawText("WASD: Mover | W/ESPACO: Pular | E/ENTER: Soco Elastico", 10, H - 60, 16, LIGHTGRAY);

    if (data->gameWon) {
        DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
        const char *winText = "VITORIA!";
        int textWidth = MeasureText(winText, 60);
        DrawText(winText, W/2 - textWidth/2, H/2 - 60, 60, BLUE);

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

static void UnloadLevelElastico(Level *level) {
    LevelElasticoData *data = (LevelElasticoData*)level->data;
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

