#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <cmath>
#include <iostream>

int main() {
    windowSetup();
    spawnTestEntities();
    while (!WindowShouldClose()) {
        SessionData::quadTree.clear();
        SessionData::quadTree.insert(&player);
        for (size_t i = 0; i < entities.size(); i++) {
            SessionData::quadTree.insert(&entities[i]);
        }
        SessionData::deltaTime = GetFrameTime();
        SessionData::deltaTime = std::fmin(SessionData::deltaTime, 0.03f);
        keyCommands();
        for (size_t i = 0; i < entities.size(); i++) entities[i].tick();
        player.tick();
        std::vector<Entity*> potentialPlayerColliders = SessionData::quadTree.queryRange(player.aabb);
        collision(&player, potentialPlayerColliders);
        monitorAndWindowChecks();
        cameraFocus();
        BeginTextureMode(SessionData::target);
        ClearBackground(SKYBLUE);
        BeginMode2D(SessionData::camera);
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