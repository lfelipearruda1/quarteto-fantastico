#include "level-common.h"
#include <stdlib.h>
#include <string.h>

void CommonUpdateInvulnerability(BaseCharacter *player) {
    if (player->invulnerabilityTimer > 0) {
        player->invulnerabilityTimer -= GetFrameTime();
    }
}

Color CommonGetPlayerColor(BaseCharacter *player) {
    if (player->invulnerabilityTimer > 0) {
        float blink = ((int)(player->invulnerabilityTimer * 10) % 2 == 0) ? 1.0f : 0.3f;
        return Fade(WHITE, blink);
    }
    return WHITE;
}

void CommonApplyGravity(BaseCharacter *player) {
    player->velocity.y += 0.6f;
    player->position.y += player->velocity.y;
    player->onGround = false;
}

void CommonCheckGroundCollision(BaseCharacter *player, float groundY) {
    if (player->position.y + player->height >= groundY) {
        player->position.y = groundY - player->height;
        player->velocity.y = 0;
        player->onGround = true;
    }
}

void CommonHandleJump(BaseCharacter *player) {
    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && player->onGround) {
        player->velocity.y = -15.0f;
        player->onGround = false;
    }
}

void CommonUpdateHorizontalMovement(BaseCharacter *player) {
    player->velocity.x = 0;
    if (IsKeyDown(KEY_A)) {
        player->velocity.x = -5.0f;
        player->facingRight = false;
    }
    if (IsKeyDown(KEY_D)) {
        player->velocity.x = 5.0f;
        player->facingRight = true;
    }
    player->position.x += player->velocity.x;
}

void CommonUpdateCamera(float *cameraX, BaseCharacter *player) {
    *cameraX = player->position.x - 200;
    if (*cameraX < 0) *cameraX = 0;
}

void CommonClampPlayerToCamera(BaseCharacter *player, float cameraX) {
    if (player->position.x < cameraX) {
        player->position.x = cameraX;
    }
    if (player->position.x + player->width > cameraX + W) {
        player->position.x = cameraX + W - player->width;
    }
}

void CommonCheckObstacleCollision(BaseCharacter *player, BaseObstacle *obstacles, int maxObstacles) {
    Rectangle playerRect = {
        player->position.x,
        player->position.y,
        player->width,
        player->height
    };

    for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].active) {
            if (CheckCollisionRecs(playerRect, obstacles[i].rect)) {
                float playerBottom = player->position.y + player->height;
                float playerTop = player->position.y;
                float obstacleTop = obstacles[i].rect.y;
                float obstacleBottom = obstacles[i].rect.y + obstacles[i].rect.height;

                if (player->velocity.y >= 0 && playerBottom - player->velocity.y <= obstacleTop + 5) {
                    player->position.y = obstacleTop - player->height;
                    player->velocity.y = 0;
                    player->onGround = true;
                } else if (player->velocity.y < 0 && playerTop - player->velocity.y >= obstacleBottom) {
                    player->position.y = obstacleBottom;
                    player->velocity.y = 0;
                } else if (player->velocity.x > 0) {
                    player->position.x = obstacles[i].rect.x - player->width;
                } else if (player->velocity.x < 0) {
                    player->position.x = obstacles[i].rect.x + obstacles[i].rect.width;
                }
            }
        }
    }
}

void CommonActivateEnemiesNearPlayer(BaseEnemy *enemies, int maxEnemies, float playerX, float spawnDistance) {
    for (int i = 0; i < maxEnemies; i++) {
        if (!enemies[i].active) {
            if (playerX + spawnDistance >= enemies[i].position.x) {
                enemies[i].active = true;
            }
        }
    }
}

void CommonUpdateEnemies(BaseEnemy *enemies, int maxEnemies, float cameraX) {
    for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active) {
            enemies[i].position.x -= enemies[i].speed;
            if (enemies[i].position.x < cameraX - 100) {
                enemies[i].active = false;
            }
        }
    }
}

bool CommonCheckPlayerEnemyCollision(BaseCharacter *player, BaseEnemy *enemy) {
    Rectangle playerRect = {
        player->position.x,
        player->position.y,
        player->width,
        player->height
    };

    Rectangle enemyRect = {
        enemy->position.x,
        enemy->position.y,
        enemy->width,
        enemy->height
    };

    return CheckCollisionRecs(playerRect, enemyRect) && player->invulnerabilityTimer <= 0;
}

void CommonHandlePlayerDamage(BaseCharacter *player, BaseEnemy *enemy, bool *gameLost) {
    player->health--;
    player->invulnerabilityTimer = 1.5f;
    enemy->active = false;
    if (player->health <= 0) {
        *gameLost = true;
    }
}

void CommonDrawHealthHearts(int health, Texture2D heartTexture) {
    for (int i = 0; i < health && i < 3; i++) {
        Rectangle source = {0, 0, (float)heartTexture.width, (float)heartTexture.height};
        Rectangle dest = {10 + i * 55, 10, 40, 40};
        DrawTexturePro(heartTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

void CommonDrawHUD(int score, float playerX, float mapLength) {
    DrawText(TextFormat("PONTOS: %d", score), 180, 20, 22, GOLD);
    DrawText(TextFormat("DISTANCIA: %.0f/%.0f", playerX, (float)mapLength), 450, 20, 20, WHITE);
}

void CommonDrawProgressBar(float progress, Color barColor) {
    DrawRectangle(10, H - 30, (int)(W - 20), 20, DARKGRAY);
    DrawRectangle(10, H - 30, (int)((W - 20) * progress), 20, barColor);
}

void CommonDrawBackground(Texture2D backgroundTexture, float cameraX) {
    float parallaxSpeed = 0.5f;
    float bgX = -(cameraX * parallaxSpeed);
    int numCopies = (int)((cameraX * parallaxSpeed + W) / backgroundTexture.width) + 2;
    
    for (int i = -1; i < numCopies; i++) {
        float drawX = bgX + (i * backgroundTexture.width);
        Rectangle source = {0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height};
        Rectangle dest = {drawX, 0, (float)backgroundTexture.width, (float)H};
        DrawTexturePro(backgroundTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

void CommonDrawPlatform(Texture2D platformTexture, float cameraX, int mapLength) {
    int tileWidth = platformTexture.width;
    int tileHeight = H - GROUND_Y;
    int startTile = (int)(cameraX / tileWidth);
    int endTile = (int)((cameraX + W) / tileWidth) + 2;

    for (int i = startTile; i < endTile && i * tileWidth < mapLength + 1000; i++) {
        Rectangle source = {0, 0, (float)platformTexture.width, (float)platformTexture.height};
        Rectangle dest = {
            i * tileWidth - cameraX,
            GROUND_Y,
            (float)tileWidth,
            (float)tileHeight
        };
        DrawTexturePro(platformTexture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}

void CommonDrawObstacles(BaseObstacle *obstacles, int maxObstacles, Texture2D obstacleTexture, float cameraX) {
    for (int i = 0; i < maxObstacles; i++) {
        if (obstacles[i].active) {
            Rectangle drawRect = obstacles[i].rect;
            drawRect.x -= cameraX;
            Rectangle source = {0, 0, (float)obstacleTexture.width, (float)obstacleTexture.height};
            DrawTexturePro(obstacleTexture, source, drawRect, (Vector2){0, 0}, 0, WHITE);
        }
    }
}

void CommonDrawVictoryScreen(int score) {
    DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
    const char *winText = "VITORIA!";
    int textWidth = MeasureText(winText, 60);
    DrawText(winText, W/2 - textWidth/2, H/2 - 60, 60, GREEN);

    const char *scoreText = TextFormat("Pontuacao Final: %d", score);
    int scoreWidth = MeasureText(scoreText, 30);
    DrawText(scoreText, W/2 - scoreWidth/2, H/2 + 20, 30, GOLD);

    const char *continueText = "Pressione ENTER para continuar...";
    int continueWidth = MeasureText(continueText, 20);
    DrawText(continueText, W/2 - continueWidth/2, H/2 + 80, 20, WHITE);
}

void CommonDrawGameOverScreen(int score) {
    DrawRectangle(0, 0, W, H, (Color){0, 0, 0, 180});
    const char *loseText = "GAME OVER!";
    int textWidth = MeasureText(loseText, 60);
    DrawText(loseText, W/2 - textWidth/2, H/2 - 60, 60, RED);

    const char *scoreText = TextFormat("Pontuacao: %d", score);
    int scoreWidth = MeasureText(scoreText, 30);
    DrawText(scoreText, W/2 - scoreWidth/2, H/2 + 20, 30, GOLD);

    const char *continueText = "Pressione ENTER para voltar...";
    int continueWidth = MeasureText(continueText, 20);
    DrawText(continueText, W/2 - continueWidth/2, H/2 + 80, 20, WHITE);
}

void CommonHandleGameEnd(bool gameWon, bool gameLost, float *gameEndTimer, int score, GameState *state) {
    if (gameWon || gameLost) {
        *gameEndTimer += GetFrameTime();

        if (IsKeyPressed(KEY_ENTER) || *gameEndTimer > 3.0f) {
            if (gameWon) {
                if (playerList != NULL) {
                    playerList->score += score;
                    UpdatePlayerScore(playerList->name, playerList->score);
                }
            }
            *state = STATE_SELECT;
        }
    }
}

bool CommonCheckVictory(float playerX, float mapLength) {
    return playerX >= mapLength;
}

bool CommonCheckFall(float playerY) {
    return playerY > H;
}

