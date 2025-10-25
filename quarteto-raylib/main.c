#include "raylib.h"
#include <math.h>

typedef enum { MENU, PLAYING } GameState;

static Color BG = (Color){10, 18, 32, 255};
static Color GOLDY = (Color){255, 208, 64, 255};

int main(void) {
    const int W = 960, H = 540;
    InitWindow(W, H, "Quarteto Fantástico — Menu");
    SetTargetFPS(60);

    Texture2D logo = LoadTexture("assets/logo.png");
    if (logo.id == 0) logo = LoadTexture("assets/logo.jpg");

    GameState state = MENU;
    float fade = 0.0f;
    float t = 0.0f;
    bool canStart = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        t += dt;

        if (state == MENU) {
            if (fade < 1.0f) {
                fade += dt * 1.2f;
                if (fade >= 1.0f) { fade = 1.0f; canStart = true; }
            } else canStart = true;
            if (canStart && IsKeyPressed(KEY_ENTER)) state = PLAYING;
        } else if (state == PLAYING) {
            if (IsKeyPressed(KEY_ESCAPE)) state = MENU;
        }

        BeginDrawing();
        ClearBackground(BG);

        if (state == MENU) {
            if (logo.id != 0) {
                float maxH = H * 0.95f;
                float scale = (logo.height > maxH) ? (maxH / (float)logo.height) : 1.0f;
                float w = logo.width * scale;
                float h = logo.height * scale;
                float x = (W - w) * 0.5f;
                float y = (H - h) * 0.30f;
                Color whiteFade = WHITE; whiteFade.a = (unsigned char)(fade * 255);
                DrawTextureEx(logo, (Vector2){ x, y }, 0.0f, scale, whiteFade);
            } else {
                const char* title = "QUARTETO FANTASTICO";
                int fs = 80;
                Vector2 m = MeasureTextEx(GetFontDefault(), title, fs, 2);
                Color c = GOLDY; c.a = (unsigned char)(fade * 255);
                DrawTextEx(GetFontDefault(), title, (Vector2){ (W - m.x)/2, H*0.25f }, fs, 2, c);
            }

            float blink = (sinf(t * 3.0f) * 0.5f + 0.5f);
            unsigned char a = (unsigned char)(blink * 255 * (canStart ? 1.0f : 0.0f) * fade);
            Color hint = RAYWHITE; hint.a = a;
            const char* press = "PRESS ENTER TO START";
            int fs2 = 28;
            int tw = MeasureText(press, fs2);
            DrawText(press, (W - tw)/2, (int)(H * 0.78f), fs2, hint);

            Color small = (Color){180, 200, 220, (unsigned char)(180 * fade)};
            DrawText("ESC para sair", 20, H - 32, 18, small);
        } else if (state == PLAYING) {
            const char* txt = "JOGO RODANDO...";
            int fs = 28;
            DrawText(txt, W/2 - MeasureText(txt, fs)/2, H/2 - 14, fs, GOLD);
            DrawText("ESC para voltar ao menu", W/2 - 160, H/2 + 24, 18, LIGHTGRAY);
        }

        EndDrawing();
    }

    if (logo.id != 0) UnloadTexture(logo);
    CloseWindow();
    return 0;
}
