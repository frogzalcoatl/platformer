#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <cmath>
#include <iostream>

int main() {
    windowSetup();
    spawnTestEntities();
    while (!WindowShouldClose()) {
        monitorAndWindowChecks();
        cameraFocus(SessionData::camera);
        SessionData::deltaTime = GetFrameTime();
        SessionData::deltaTime = std::fmin(SessionData::deltaTime, 0.03f);
        keyCommands(SessionData::command);
        SessionData::quadTree.clear();
        SessionData::quadTree.insert(&player);
        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(SessionData::camera);
        for (size_t i = 0; i < entities.size(); i++) {
            SessionData::quadTree.insert(&entities[i]);
            entities[i].tick();
            std::vector<Entity*> potentialEntityColliders = SessionData::quadTree.queryRange(entities[i].aabb);
            collision(&entities[i], potentialEntityColliders);
            entities[i].draw();
        }
        player.tick();
        std::vector<Entity*> potentialPlayerColliders = SessionData::quadTree.queryRange(player.aabb);
        collision(&player, potentialPlayerColliders);
        player.draw();
        //SessionData::quadTree.draw();
        EndMode2D();
        if (GameConfig::showDebugInfo) displayDebugInfo();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}