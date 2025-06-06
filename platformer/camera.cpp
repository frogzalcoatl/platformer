#include "sahars_platformer.h"
#include <algorithm>
#include <cmath>

void windowSetup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    MaximizeWindow();
    monitorAndWindowChecks(true);
    SessionData::camera.rotation = 0.0f;
    SessionData::camera.zoom = 1.0f;
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
void cameraFocus(Camera2D &camera) {
    // Funky -y position stuff happening here for better entity y positions. up = +y and down = -y instead of the opposite.
    if (!GameConfig::cameraShouldFollow) return;
    float visibleWorldWidth = SessionData::windowWidth / camera.zoom;
    float visibleWorldHeight = SessionData::windowHeight / camera.zoom;
    float minCameraFocusX = -GameConfig::GROUND_WIDTH * SessionData::pixelsPerBlock / 2.0f + (visibleWorldWidth / 2.0f);
    float maxCameraFocusX = GameConfig::GROUND_WIDTH * SessionData::pixelsPerBlock / 2.0f - (visibleWorldWidth / 2.0f);
    float minCameraFocusY = -1e7;
    float maxCameraFocusY = SessionData::windowHeight / 2.0f;
    float playerRelativeX = player.position.x * SessionData::pixelsPerBlock - camera.target.x;
    float playerRelativeY = 2 * SessionData::windowHeight / 2.0f - player.position.y * SessionData::pixelsPerBlock - camera.target.y;
    float deadZoneWorldLimitX = (visibleWorldWidth * (GameConfig::DEAD_ZONE_PERCENT_X)) / 2.0f;
    float deadZoneWorldLimitY = (visibleWorldHeight * (GameConfig::DEAD_ZONE_PERCENT_Y)) / 2.0f;
    if (playerRelativeX > deadZoneWorldLimitX) {
        camera.target.x += playerRelativeX - deadZoneWorldLimitX;
    } else if (playerRelativeX < -deadZoneWorldLimitX) {
        camera.target.x += playerRelativeX + deadZoneWorldLimitX;
    }
    if (playerRelativeY > deadZoneWorldLimitY) {
        camera.target.y += playerRelativeY - deadZoneWorldLimitY;
    } else if (playerRelativeY < -deadZoneWorldLimitY) {
        camera.target.y += playerRelativeY + deadZoneWorldLimitY;
    }
    camera.target.x = minCameraFocusX <= maxCameraFocusX ? std::clamp(camera.target.x, minCameraFocusX, maxCameraFocusX) : 0.0f;
    camera.target.y = minCameraFocusY <= maxCameraFocusY ? std::clamp(camera.target.y, minCameraFocusY, maxCameraFocusY) : 0.0f;
}
void displayDebugInfo() {
    DrawFPS(10, 10);
    DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", SessionData::windowWidth, SessionData::windowHeight, SessionData::windowAspectRatio), 10, 30, 20, LIME);
    DrawText(TextFormat("Player Pos: %.2f, %.2f", player.position.x, player.position.y), 10, 55, 20, BLACK);
    DrawText(TextFormat("Camera Pos: %.2f, %.2f", SessionData::camera.target.x / SessionData::pixelsPerBlock, -SessionData::camera.target.y / SessionData::pixelsPerBlock), 10, 80, 20, BLACK);
    DrawText(TextFormat("Command: %d", SessionData::command), 10, 105, 20, BLACK);
    DrawText(TextFormat("Camera Zoom: %.2fx", SessionData::camera.zoom), 10, 130, 20, BLACK);
    DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 155, 20, BLACK);
    DrawText(player.isGrounded ? "isGrounded: true" : "isGrounded: false", 10, 180, 20, BLACK);
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
void platformerDrawAABB(const AABB* aabb, Color color) {
    DrawRectangleV({ aabb->min.x * SessionData::pixelsPerBlock, -aabb->max.y * SessionData::pixelsPerBlock + SessionData::windowHeight }, { (aabb->max.x - aabb->min.x) * SessionData::pixelsPerBlock, (aabb->max.y - aabb->min.y) * SessionData::pixelsPerBlock }, color);
}
void platformerDrawAABBLines(const AABB* aabb, float lineThick, Color color) {
    DrawRectangleLinesEx({ aabb->min.x * SessionData::pixelsPerBlock, -aabb->max.y * SessionData::pixelsPerBlock + SessionData::windowHeight, (aabb->max.x - aabb->min.x) * SessionData::pixelsPerBlock, (aabb->max.y - aabb->min.y) * SessionData::pixelsPerBlock }, lineThick * SessionData::pixelsPerBlock, color);
}
void platformerDrawText(const char* text, int posX, int posY, int fontSize, Color color, const Vector2* textDimensionPtr) {
    if (!textDimensionPtr) {
        Vector2 textDimensions = MeasureTextEx(GetFontDefault(), text, (float)fontSize, TEXT_SPACING);
        DrawText(text, posX * SessionData::pixelsPerBlock, -(posY + (int)textDimensions.y) * SessionData::pixelsPerBlock + SessionData::windowHeight, fontSize * SessionData::pixelsPerBlock, color);
        return;
    }
    DrawText(text, posX * SessionData::pixelsPerBlock, -(posY + (int)textDimensionPtr->y) * SessionData::pixelsPerBlock + SessionData::windowHeight, fontSize * SessionData::pixelsPerBlock, color);
}