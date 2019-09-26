#include "stdint.h"
#include <math.h>
#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    //Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    const uint16_t vx1[20] = {63, 63, 20, 36, 60, 90, 90};
    const uint16_t vy1[20] = {20, 60, 50, 40, 0,  20, 10};
    const uint16_t vx2[20] = {63, 20, 36, 60, 90, 63, 90};
    const uint16_t vy2[20] = {60, 50, 40, 0,  10, 20, 20};
    float tx1[20], ty1[20], tz1[20], tx2[20], ty2[20], tz2[20];

    const Color wc[20] = {DARKGRAY, DARKGRAY, DARKGRAY, DARKGRAY, DARKGRAY, DARKGRAY, DARKGRAY, DARKGRAY};
    const uint16_t totalWalls = 8;
    float px = 80;
    float py = 15;
    float angle = 0;

    InitWindow(screenWidth, screenHeight, "wolf3d clone");
    RenderTexture2D absoluteMap = LoadRenderTexture(114, 2*64);
    RenderTexture2D transformedMap = LoadRenderTexture(114, 2*64);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_RIGHT)) angle += 0.1f;
        if (IsKeyDown(KEY_LEFT)) angle -= 0.1f;
        if (IsKeyDown(KEY_UP)) {
          px += cos(angle);
          py += sin(angle);
        }
        if (IsKeyDown(KEY_DOWN)) {
          px -= cos(angle);
          py -= sin(angle);
        }

        ClearBackground(RAYWHITE);

        BeginDrawing();
          BeginTextureMode(absoluteMap);
            ClearBackground(LIGHTGRAY);

            DrawLine(px, py, px + 5 * cos(angle), py + 5 * sin(angle), DARKBLUE);
            for(uint16_t w=0; w<totalWalls; w++) {
              DrawLine(vx1[w], vy1[w], vx2[w], vy2[w], wc[w]);
            }
            DrawCircle(px, py, 1, DARKBLUE);
          EndTextureMode();

          BeginTextureMode(transformedMap);
            ClearBackground(LIGHTGRAY);
            for(uint16_t w=0; w<totalWalls; w++) {
              tx1[w] = vx1[w] - px;
              ty1[w] = vy1[w] - py;
              tx2[w] = vx2[w] - px;
              ty2[w] = vy2[w] - py;
              tz1[w] = (tx1[w] * cos(angle)) + (ty1[w] * sin(angle));
              tz2[w] = (tx2[w] * cos(angle)) + (ty2[w] * sin(angle));
              tx1[w] = (tx1[w] * sin(angle)) - (ty1[w] * cos(angle));
              tx2[w] = (tx2[w] * sin(angle)) - (ty2[w] * cos(angle));
              DrawLine(50 - tx1[w], 50 - tz1[w], 50 - tx2[w], 50 - tz2[w], wc[w]);
            }
            DrawLine(50, 50, 50, 45, DARKBLUE);
            DrawCircle(50, 50, 1, DARKBLUE);
          EndTextureMode();

          DrawTexturePro(absoluteMap.texture, (Rectangle){ 0, 0, absoluteMap.texture.width, -absoluteMap.texture.height }, (Rectangle){ 0, 0, screenWidth, 2*screenHeight }, (Vector2){ 0.0f, 0.0f }, 0, WHITE);
          DrawTexturePro(transformedMap.texture, (Rectangle){ 0, 0, transformedMap.texture.width, -transformedMap.texture.height }, (Rectangle){ screenWidth, 0, screenWidth, 2*screenHeight }, (Vector2){ 0.0f, 0.0f }, 0, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(absoluteMap);
    UnloadRenderTexture(transformedMap);
    CloseWindow();

    return 0;
}
