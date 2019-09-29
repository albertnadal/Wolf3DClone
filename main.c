#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"

#define FNcross(X1, Y1, X2, Y2)  ((X1 * Y2) - (Y1 * X2))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

void intersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* x, float* y) {
    float _x = FNcross(x1, y1, x2, y2);
    float _y = FNcross(x3, y3, x4, y4);
    float det = FNcross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    *x = FNcross(_x, x1 - x2, _y, x3 - x4) / det;
    *y = FNcross(_x, y1 - y2, _y, y3 - y4) / det;
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    //Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2 };
    const uint16_t vx1[20] = {63, 63, 20, 36, 60, 90, 90};
    const uint16_t vy1[20] = {20, 60, 50, 40, 0,  20, 10};
    const uint16_t vx2[20] = {63, 20, 36, 60, 90, 63, 90};
    const uint16_t vy2[20] = {60, 50, 40, 0,  10, 20, 20};
    float tx1[20], ty1[20], tz1[20], tx2[20], ty2[20], tz2[20];

    const Color wc[20] = {ORANGE, MAROON, BLUE, BROWN, PINK, GREEN, YELLOW, DARKGRAY};
    const uint16_t totalWalls = 8;
    float px = 80;
    float py = 15;
    float angle = 0;

    InitWindow(screenWidth, screenHeight, "wolf3d clone");
    RenderTexture2D absoluteMap = LoadRenderTexture(114, 2*64);
    RenderTexture2D transformedMap = LoadRenderTexture(114, 2*64);
    RenderTexture2D perspectiveMap = LoadRenderTexture(114, 2*64);

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
            //DrawText("some basic shapes available on raylib", 20, 20, 20, DARKGRAY);
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

          BeginTextureMode(perspectiveMap);
            ClearBackground(WHITE);
            float ix1, ix2, iz1, iz2, x1, y1a, y1b, x2, y2a, y2b, min, max, ya, yb;
            for(uint16_t w=0; w<totalWalls; w++) {
              if((tz1[w] > 0.0f) || (tz2[w] > 0.0f)) {
                intersect(tx1[w], tz1[w], tx2[w], tz2[w], -0.0001f, 0.0001f, -10.0f, 5.0f, &ix1, &iz1);
                intersect(tx1[w], tz1[w], tx2[w], tz2[w], 0.0001f, 0.0001f, 10.0f, 5.0f, &ix2, &iz2);

                if(tz1[w] <= 0.0f) {
                  if(iz1 > 0.0f) {
                    tx1[w] = ix1;
                    tz1[w] = iz1;
                  } else {
                    tx1[w] = ix2;
                    tz1[w] = iz2;
                  }
                }

                if(tz2[w] <= 0.0f) {
                  if(iz1 > 0.0f) {
                    tx2[w] = ix1;
                    tz2[w] = iz1;
                  } else {
                    tx2[w] = ix2;
                    tz2[w] = iz2;
                  }
                }

                if((tz1[w] < 0.0f) || (tz2[w] < 0.0f)) {
                  continue;
                }

                x1 = (-tx1[w] * 74.0f) / tz1[w];
                x2 = (-tx2[w] * 74.0f) / tz2[w];
                y1a = -140.0f / tz1[w];
                y1b = 140.0f / tz1[w];
                y2a = -140.0f / tz2[w];
                y2b = 140.0f / tz2[w];

                if(x2 >= x1) {
                  min = MAX(x1, -50.0f);
                  max = MIN(x2, 50.0f);
                  //printf("w: %d x1: %f x2: %f min: %f  max: %f\n", w, x1, x2, min, max);
                  for(float x=min; x<=max; x++) {
                    ya = y1a + (((x-x1) * (y2a-y1a)) / (x2-x1+1));
                    yb = y1b + (((x-x1) * (y2b-y1b)) / (x2-x1+1));
                    //printf("DrawLine(50 + %f, 50 + %f, 50 + %f, 50 + %f\n", x, ya, x, yb);
                    DrawLine(50.0f + x, 50.0f + ya, 50.0f + x, 50.0f + yb, wc[w]);
                  }
                  DrawLine(50 + x1, 50 + y1a, 50 + x1, 50 + y1b, wc[w]);
                  DrawLine(50 + x2, 50 + y2a, 50 + x2, 50 + y2b, wc[w]);
                }
              }
            }
          EndTextureMode();

          DrawTexturePro(absoluteMap.texture, (Rectangle){ 0, 0, absoluteMap.texture.width, -absoluteMap.texture.height }, (Rectangle){ 0, 0, screenWidth, screenHeight }, (Vector2){ 0.0f, 0.0f }, 0, WHITE);
          DrawTexturePro(transformedMap.texture, (Rectangle){ 0, 0, transformedMap.texture.width, -transformedMap.texture.height }, (Rectangle){ screenWidth, 0, screenWidth, screenHeight }, (Vector2){ 0.0f, 0.0f }, 0, WHITE);
          DrawTexturePro(perspectiveMap.texture, (Rectangle){ 0, 0, perspectiveMap.texture.width, -perspectiveMap.texture.height }, (Rectangle){ 0, screenHeight, 2*screenWidth, screenHeight }, (Vector2){ 0.0f, 0.0f }, 0, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(absoluteMap);
    UnloadRenderTexture(transformedMap);
    UnloadRenderTexture(perspectiveMap);
    CloseWindow();

    return 0;
}
