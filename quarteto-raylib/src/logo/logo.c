#include "logo.h"
#include "raylib.h"

static Texture2D logo;
static float fade = 0.0f;
static float timerLogo = 0.0f;

void InitLogo(void) {
    logo = LoadTexture("assets/logo.png");
    fade = 0.0f;
    timerLogo = 0.0f;
}

void UpdateLogo(GameState *state) {
    float dt = GetFrameTime();
    timerLogo += dt;
    if (fade < 1.0f) fade += dt * 0.8f;

    if (timerLogo > 2.0f && IsKeyPressed(KEY_ENTER)) {
        *state = STATE_SELECT;
    }

    if (timerLogo > 5.0f) {
        *state = STATE_SELECT;
    }
}

void DrawLogo(void) {
    ClearBackground((Color){10, 18, 32, 255});
    DrawTexture(logo, W/2 - logo.width/2, H/2 - logo.height/2, Fade(WHITE, fade));
    if (timerLogo > 2.0f) {
        DrawText("PRESSIONE ENTER", W/2 - MeasureText("PRESSIONE ENTER", 20)/2, H*0.8f, 20, Fade(WHITE, fade));
    }
}

void UnloadLogo(void) {
    UnloadTexture(logo);
}

