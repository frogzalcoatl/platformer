#include "sahars_platformer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

void windowSetup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    SetExitKey(KEY_NULL);
    MaximizeWindow();
    monitorAndWindowChecks(true);
    SessionData::camera.rotation = 0.0f;
    SessionData::camera.zoom = 1.0f;
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x00000055);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x11111188);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x22222288);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0xAAAAAAAA);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0xDDDDDDDD);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0xFFFFFFFF);
}
void monitorAndWindowChecks(bool overRideWindowResizedCheck) {
    if (GetCurrentMonitor() != SessionData::currentMonitor) {
        SessionData::currentMonitor = GetCurrentMonitor();
        SessionData::monitorAspectRatio = (float)GetMonitorWidth(SessionData::currentMonitor) / GetMonitorHeight(SessionData::currentMonitor);
    }
    if (GetMonitorRefreshRate(SessionData::currentMonitor) != SessionData::monitorRefreshRate) {
        SessionData::monitorRefreshRate = GetMonitorRefreshRate(SessionData::currentMonitor);
        SetTargetFPS(SessionData::monitorRefreshRate);
    }
    if (IsWindowResized() || overRideWindowResizedCheck) {
        SessionData::windowWidth = GetScreenWidth();
        SessionData::windowHeight = GetScreenHeight();
        SessionData::windowAspectRatio = (float)SessionData::windowWidth / (float)SessionData::windowHeight;
        SessionData::pixelsPerBlock = SessionData::windowWidth / 80.0f;
        SessionData::camera.offset = { SessionData::windowWidth / 2.0f, SessionData::windowHeight / 2.0f };
    }
}
void cameraFollow(Camera2D& camera) {
    camera.offset = { SessionData::windowWidth / 2.0f, SessionData::windowHeight / 2.0f };
    camera.target = { player.position.x, -player.position.y };
    float worldScreenWidth = SessionData::windowWidth / camera.zoom;
    float worldScreenHeight = SessionData::windowHeight / camera.zoom;
    Vector2 minTargetPos = { GameConfig::WORLD_LEFT_BORDER + worldScreenWidth / 2.0f, -GameConfig::WORLD_CEILING_Y + worldScreenHeight / 2.0f };
    Vector2 maxTargetPos = { GameConfig::WORLD_RIGHT_BORDER - worldScreenWidth / 2.0f, -GameConfig::WORLD_FLOOR_Y - worldScreenHeight / 2.0f };
    if (minTargetPos.x > maxTargetPos.x) {
        camera.target.x = 0.0f;
    } else {
        camera.target.x = Clamp(camera.target.x, minTargetPos.x, maxTargetPos.x);
    }
    if (minTargetPos.y > maxTargetPos.y) {
        camera.target.y = -SessionData::windowHeight / 2.0f / camera.zoom;
    } else {
        camera.target.y = Clamp(camera.target.y, minTargetPos.y, maxTargetPos.y);
    }

}
/*Smooth Implementation
void cameraFollow(Camera2D& camera) {
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 2.0f;
    Vector2 diff = Vector2Subtract({ player.position.x, -player.position.y }, camera.target);
    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera.target = Vector2Add(camera.target, Vector2Scale(diff, speed * SessionData::deltaTime / length));
    }
    float worldScreenWidth = SessionData::windowWidth / camera.zoom;
    float worldScreenHeight = SessionData::windowHeight / camera.zoom;
    Vector2 minTargetPos = {
        GameConfig::WORLD_LEFT_BORDER + worldScreenWidth / 2.0f,
        -GameConfig::WORLD_CEILING_Y + worldScreenHeight / 2.0f
    };
    Vector2 maxTargetPos = {
        GameConfig::WORLD_RIGHT_BORDER - worldScreenWidth / 2.0f,
        -GameConfig::WORLD_FLOOR_Y - worldScreenHeight / 2.0f
    };
    if (minTargetPos.x > maxTargetPos.x) {
        camera.target.x = 0.0f;
    } else {
        camera.target.x = Clamp(camera.target.x, minTargetPos.x, maxTargetPos.x);
    }
    if (minTargetPos.y > maxTargetPos.y) {
        camera.target.y = -SessionData::windowHeight / 2.0f / camera.zoom;
    } else {
        camera.target.y = Clamp(camera.target.y, minTargetPos.y, maxTargetPos.y);
    }
}*/
void cameraStatic(Camera2D& camera) {
    if (SessionData::leftClickDown || SessionData::rightClickDown) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }
}
void displayDebugInfo() {
    DrawFPS(10, 10);
    DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", SessionData::windowWidth, SessionData::windowHeight, SessionData::windowAspectRatio), 10, 30, 20, LIME);
    DrawText(TextFormat("Player Pos: %.2f, %.2f", player.position.x, player.position.y), 10, 55, 20, BLACK);
    DrawText(TextFormat("Camera Pos: %.2f, %.2f", SessionData::camera.target.x, -SessionData::camera.target.y), 10, 80, 20, BLACK);
    DrawText(TextFormat("User Zoom: %.2fx", SessionData::userZoom), 10, 105, 20, BLACK);
    DrawText(SessionData::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC (Drag Mouse)", 10, 130, 20, BLACK);
}
void platformerFullscreenToggle() {
    if (IsWindowFullscreen()) {
        SetWindowSize(SessionData::preFullscreenWindowWidth, SessionData::preFullscreenWindowHeight);
    } else {
        SessionData::preFullscreenWindowWidth = SessionData::windowWidth;
        SessionData::preFullscreenWindowHeight = SessionData::windowHeight;
        SetWindowSize(GetMonitorWidth(SessionData::currentMonitor), GetMonitorHeight(SessionData::currentMonitor));
    }
    ToggleFullscreen();
}
void drawAABB(const AABB* aabb, Color color) {
    DrawRectangleV({ aabb->min.x, -aabb->max.y }, { (aabb->max.x - aabb->min.x), (aabb->max.y - aabb->min.y) }, color);
}
void drawAABBLines(const AABB* aabb, float lineThick, Color color) {
    DrawRectangleLinesEx({ aabb->min.x, -aabb->max.y, (aabb->max.x - aabb->min.x), (aabb->max.y - aabb->min.y) }, lineThick, color);
}
void drawWorldText(const char* text, float posX, float posY, float fontSize, float spacing, Color color) {
    Vector2 screenPosition = GetWorldToScreen2D({ posX, -posY }, SessionData::camera);
    float scaledFontSize = fontSize * SessionData::userZoom;
    float scaledSpacing = spacing * SessionData::userZoom;
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, scaledFontSize, scaledSpacing);
    Vector2 textTopLeft = { screenPosition.x - textSize.x / 2.0f, screenPosition.y - textSize.y / 2.0f };
    DrawTextEx(GetFontDefault(), text, textTopLeft, scaledFontSize, scaledSpacing, color);
}
void drawGameCamera() {
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode2D(SessionData::camera);
    for (size_t i = 0; i < entities.size(); i++) entities[i].draw();
    player.draw();
    SessionData::quadTree.drawBounds();
    EndMode2D();
    SessionData::quadTree.drawText();
}