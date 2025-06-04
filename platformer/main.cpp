#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <cmath>
#include <iostream>

int main() {
    windowSetup();
    spawnTestEntities();
    player.moveTo(0.0f, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
    while (!WindowShouldClose()) {
        SessionData::deltaTime = GetFrameTime();
        SessionData::deltaTime = std::fmin(SessionData::deltaTime, 0.03f);
        keyCommands();
        //SessionData::quadTree.queryRange(SessionData::quadTree.boundary);
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].tick();
        for (size_t i = 0; i < entities.size(); i++) entities[i].tick();
        player.tick();
        monitorAndWindowChecks();
        cameraFocus();
        BeginTextureMode(SessionData::target);
        ClearBackground(SKYBLUE);
        BeginMode2D(SessionData::gameCamera);
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].draw();
        for (size_t i = 0; i < entities.size(); i++) entities[i].draw();
        player.draw();
        //SessionData::quadTree.draw();
        EndMode2D();
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledScreen();
        if (GameConfig::showDebugInfo) displayDebugInfo();
        EndDrawing();
    }
    UnloadRenderTexture(SessionData::target);
    CloseWindow();
    return 0;
}