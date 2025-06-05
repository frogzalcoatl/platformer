#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <cmath>
#include <iostream>

int main() {
    windowSetup();
    spawnTestEntities();
    player.moveTo(GameConfig::PLAYER_SPAWN_POINT.x, GameConfig::PLAYER_SPAWN_POINT.y);
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
        BeginMode2D(SessionData::camera);
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].draw();
        for (size_t i = 0; i < entities.size(); i++) entities[i].draw();
        player.draw();
        SessionData::quadTree.draw();
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