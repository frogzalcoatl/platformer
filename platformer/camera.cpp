#include "sahars_platformer.h"
#include <algorithm>
#include <cmath>

void windowSetup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    MaximizeWindow();
    SetTextureWrap(SessionData::target.texture, TEXTURE_WRAP_CLAMP);
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
        SessionData::pixelsPerBlock = SessionData::windowWidth / 80;
        SessionData::camera.target = { player.position.x * SessionData::pixelsPerBlock, player.position.y * SessionData::pixelsPerBlock };
        SessionData::camera.offset = { SessionData::windowWidth / 2.0f, SessionData::windowHeight / 2.0f };
        SessionData::target = LoadRenderTexture(SessionData::windowWidth, SessionData::windowHeight);
    }
}
void cameraFocus() {
    if (!GameConfig::cameraShouldFollow) return;
    float visibleWorldWidth = SessionData::windowWidth / SessionData::camera.zoom;
    float visibleWorldHeight = SessionData::windowHeight / SessionData::camera.zoom;
    SessionData::minCameraFocusX = -GameConfig::GROUND_WIDTH * SessionData::pixelsPerBlock / 2.0f + (visibleWorldWidth / 2.0f);
    SessionData::maxCameraFocusX = GameConfig::GROUND_WIDTH * SessionData::pixelsPerBlock / 2.0f - (visibleWorldWidth / 2.0f);
    SessionData::minCameraFocusY = (float)SessionData::windowHeight / 2.0f - GameConfig::GROUND_HEIGHT;
    SessionData::maxCameraFocusY = 1000000.0f;
    float playerRelativeX = player.position.x * SessionData::pixelsPerBlock - SessionData::camera.target.x;
    float playerRelativeY = player.position.y * SessionData::pixelsPerBlock - SessionData::camera.target.y;
    float deadZoneWorldLimitX = (visibleWorldWidth * (GameConfig::DEAD_ZONE_PERCENT_X)) / 2.0f;
    float deadZoneWorldLimitY = (visibleWorldHeight * (GameConfig::DEAD_ZONE_PERCENT_Y)) / 2.0f;
    if (playerRelativeX > deadZoneWorldLimitX) {
        SessionData::camera.target.x += playerRelativeX - deadZoneWorldLimitX;
    } else if (playerRelativeX < -deadZoneWorldLimitX) {
        SessionData::camera.target.x += playerRelativeX + deadZoneWorldLimitX;
    }
    if (playerRelativeY > deadZoneWorldLimitY) {
        SessionData::camera.target.y += playerRelativeY - deadZoneWorldLimitY;
    } else if (playerRelativeY < -deadZoneWorldLimitY) {
        SessionData::camera.target.y += playerRelativeY + deadZoneWorldLimitY;
    }
    if (SessionData::minCameraFocusX <= SessionData::maxCameraFocusX) {
        SessionData::camera.target.x = std::clamp(SessionData::camera.target.x, SessionData::minCameraFocusX, SessionData::maxCameraFocusX);
    } else {
        SessionData::camera.target.x = 0.0f;
    }
    if (SessionData::minCameraFocusY <= SessionData::maxCameraFocusY) {
        SessionData::camera.target.y = std::clamp(SessionData::camera.target.y, SessionData::minCameraFocusY, SessionData::maxCameraFocusY);
    } else {
        SessionData::camera.target.y = 0.0f;
    }
}
void drawScaledScreen() {
    Rectangle destination = { 0.0f, 0.0f, (float)SessionData::windowWidth, (float)SessionData::windowHeight };
    DrawTexturePro(SessionData::target.texture, destination, destination, { 0, 0 }, 0.0f, WHITE);
}
void displayDebugInfo() {
    DrawFPS(10, 10);
    DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", SessionData::windowWidth, SessionData::windowHeight, SessionData::windowAspectRatio), 10, 30, 20, LIME);
    DrawText(TextFormat("Player Pos: %.3f, %.3f", player.position.x, player.position.y), 10, 55, 20, BLACK);
    DrawText(TextFormat("Command: %d", SessionData::command), 10, 80, 20, BLACK);
    DrawText(TextFormat("Camera Zoom: %.2fx", SessionData::camera.zoom), 10, 105, 20, BLACK);
    DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 130, 20, BLACK);
    DrawText(player.isGrounded ? "isGrounded: true" : "isGrounded: false", 10, 155, 20, BLACK);
}
void PlatformerFullscreenToggle() {
    if (IsWindowFullscreen()) {
        SetWindowSize(SessionData::preFullscreenWindowWidth, SessionData::preFullscreenWindowHeight);
    } else {
        SessionData::preFullscreenWindowWidth = SessionData::windowWidth;
        SessionData::preFullscreenWindowHeight = SessionData::windowHeight;
        SetWindowSize(GetMonitorWidth(SessionData::currentMonitor), GetMonitorHeight(SessionData::currentMonitor));
    }
    ToggleFullscreen();
}