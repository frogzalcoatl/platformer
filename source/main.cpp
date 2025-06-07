#define RAYGUI_IMPLEMENTATION
#include "sahars_platformer.h"
#include <cmath>
#include <iostream>

int WinMain() {
  windowSetup();
  spawnTestEntities();
  while (!SessionData::exitWindow) {
    if (WindowShouldClose()) SessionData::exitWindow = true;
    SessionData::deltaTime = GetFrameTime();
    SessionData::deltaTime = std::fmin(SessionData::deltaTime, 0.03f);
    userInput();
    monitorAndWindowChecks();
    if (gamePause()) continue;
    SessionData::quadTree.clear();
    SessionData::quadTree.insert(&player);
    for (size_t i = 0; i < entities.size(); i++) {
      SessionData::quadTree.insert(&entities[i]);
    }
    player.tick();
    std::vector<Entity*> potentialPlayerColliders =
        SessionData::quadTree.queryRange(player.aabb);
    collision(&player, potentialPlayerColliders);
    for (size_t i = 0; i < entities.size(); i++) {
      entities[i].tick();
      std::vector<Entity*> potentialEntityColliders =
          SessionData::quadTree.queryRange(entities[i].aabb);
      collision(&entities[i], potentialEntityColliders);
    }
    SessionData::camera.zoom =
        SessionData::pixelsPerBlock * SessionData::userZoom;
    if (SessionData::cameraShouldFollow) {
      cameraFollow(SessionData::camera);
    } else {
      cameraStatic(SessionData::camera);
    }
    drawGameCamera();
    if (SessionData::showDebugInfo) displayDebugInfo();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
bool gamePause() {
  if (SessionData::uiScreen == 0) return false;
  drawGameCamera();
  DrawRectangle(0, 0, SessionData::windowWidth, SessionData::windowHeight,
                {128, 128, 128, 128});
  if (SessionData::showDebugInfo) displayDebugInfo();
  Vector2 textSize = MeasureTextEx(GetFontDefault(), "Game Paused",
                                   SessionData::windowWidth / 15.0f, 1.0f);
  DrawTextEx(GetFontDefault(), "Game Paused",
             {SessionData::windowWidth / 2.0f - textSize.x / 2.0f,
              SessionData::windowHeight / 4.0f - textSize.y / 2.0f},
             SessionData::windowWidth / 15.0f, 1.0f, BLACK);
  GuiSetStyle(DEFAULT, TEXT_SIZE, (int)textSize.y / 2);
  GuiSetStyle(DEFAULT, TEXT_SPACING, (int)SessionData::windowWidth / 250);
  if (SessionData::uiScreen == 1) {
    if (GuiButton({SessionData::windowWidth / 2.0f - textSize.x / 2.0f,
                   SessionData::windowHeight / 2.0f - textSize.y, textSize.x,
                   textSize.y / 2.0f},
                  "Resume")) {
      SessionData::uiScreen = 0;
    }
    if (GuiButton({SessionData::windowWidth / 2.0f - textSize.x / 2.0f,
                   SessionData::windowHeight / 2.0f - textSize.y / 4.0f,
                   textSize.x, textSize.y / 2.0f},
                  "Settings")) {
      SessionData::uiScreen = 2;
    }
    if (GuiButton({SessionData::windowWidth / 2.0f - textSize.x / 2.0f,
                   SessionData::windowHeight / 2.0f + textSize.y / 2.0f,
                   textSize.x, textSize.y / 2.0f},
                  "Exit")) {
      SessionData::exitWindow = true;
    }
  } else if (SessionData::uiScreen == 2) {
    if (GuiButton({SessionData::windowWidth / 2.0f - textSize.x / 2.0f,
                   SessionData::windowHeight / 2.0f - textSize.y, textSize.x,
                   textSize.y / 2.0f},
                  "Back")) {
      SessionData::uiScreen = 1;
    }
    const char* text =
        "Jump: W\nMove Left: A\nMove Right: D\nMove Down: S\nRespawn: R\nReset "
        "Velocity: V\nToggle Gravity: G\nZoom: Mouse Scrool\nReset Zoom: "
        "Ctrl+0\nSwap Camera Follow Style: LEFT SHIFT\nToggle Debug: F3";
    DrawText(text, (int)(SessionData::windowWidth / 2.0f - textSize.x / 2.0f),
             (int)(SessionData::windowHeight / 2.0f - textSize.y / 4.0f),
             textSize.y / 3, BLACK);
  }
  EndDrawing();
  return true;
}