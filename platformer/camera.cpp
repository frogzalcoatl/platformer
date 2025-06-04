#include "sahars_platformer.h"
#include <algorithm>
#include <cmath>

void windowSetup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    MaximizeWindow();
    monitorAndWindowChecks(true);
    SessionData::gameCamera.target = player.position;
    SessionData::gameCamera.offset = { GameConfig::SOURCE_WIDTH / 2.0f, GameConfig::SOURCE_HEIGHT / 2.0f };
    SessionData::gameCamera.rotation = 0.0f;
    SessionData::gameCamera.zoom = 1.0f;
    SessionData::target = LoadRenderTexture(GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT);
    SetTextureWrap(SessionData::target.texture, TEXTURE_WRAP_CLAMP);
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
    }
}
void cameraFocus() {
    SessionData::effectiveSource = { 0.0f, 0.0f, (float)GameConfig::SOURCE_WIDTH, (float)GameConfig::SOURCE_HEIGHT };
    if (SessionData::windowAspectRatio > GameConfig::SOURCE_ASPECT_RATIO) {
        SessionData::effectiveSource.height = std::roundf((float)GameConfig::SOURCE_WIDTH / SessionData::windowAspectRatio);
        SessionData::effectiveSource.y = std::roundf(((float)GameConfig::SOURCE_HEIGHT - SessionData::effectiveSource.height) / 2.0f);
    } else if (SessionData::windowAspectRatio < GameConfig::SOURCE_ASPECT_RATIO) {
        SessionData::effectiveSource.width = std::roundf((float)GameConfig::SOURCE_HEIGHT * SessionData::windowAspectRatio);
        SessionData::effectiveSource.x = std::roundf(((float)GameConfig::SOURCE_WIDTH - SessionData::effectiveSource.width) / 2.0f);
    }
    if (GameConfig::cameraShouldFollow && !GameConfig::prevCameraFollowState) {
        SessionData::gameCamera.target = player.position;
    }
    GameConfig::prevCameraFollowState = GameConfig::cameraShouldFollow;
    if (!GameConfig::cameraShouldFollow) return;
    float visibleWorldWidth = SessionData::effectiveSource.width / SessionData::gameCamera.zoom;
    float visibleWorldHeight = SessionData::effectiveSource.height / SessionData::gameCamera.zoom;
    SessionData::minCameraFocusX = -GameConfig::GROUND_WIDTH / 2.0f + (visibleWorldWidth / 2.0f);
    SessionData::maxCameraFocusX = GameConfig::GROUND_WIDTH / 2.0f - (visibleWorldWidth / 2.0f);
    SessionData::minCameraFocusY = (float)GameConfig::SOURCE_HEIGHT / 2.0f;
    SessionData::maxCameraFocusY = (float)GameConfig::SOURCE_HEIGHT / 2.0f;
    float playerRelativeX = player.position.x - SessionData::gameCamera.target.x;
    float playerRelativeY = player.position.y - SessionData::gameCamera.target.y;
    float deadZoneWorldLimitX = (visibleWorldWidth * (GameConfig::DEAD_ZONE_PERCENT_X)) / 2.0f;
    float deadZoneWorldLimitY = (visibleWorldHeight * (GameConfig::DEAD_ZONE_PERCENT_Y)) / 2.0f;
    if (playerRelativeX > deadZoneWorldLimitX) {
        SessionData::gameCamera.target.x += playerRelativeX - deadZoneWorldLimitX;
    } else if (playerRelativeX < -deadZoneWorldLimitX) {
        SessionData::gameCamera.target.x += playerRelativeX + deadZoneWorldLimitX;
    }
    if (playerRelativeY > deadZoneWorldLimitY) {
        SessionData::gameCamera.target.y += playerRelativeY - deadZoneWorldLimitY;
    } else if (playerRelativeY < -deadZoneWorldLimitY) {
        SessionData::gameCamera.target.y += playerRelativeY + deadZoneWorldLimitY;
    }
    if (SessionData::minCameraFocusX <= SessionData::maxCameraFocusX) {
        SessionData::gameCamera.target.x = std::clamp(SessionData::gameCamera.target.x, SessionData::minCameraFocusX, SessionData::maxCameraFocusX);
    } else {
        SessionData::gameCamera.target.x = 0.0f;
    }
    if (SessionData::minCameraFocusY <= SessionData::maxCameraFocusY) {
        SessionData::gameCamera.target.y = std::clamp(SessionData::gameCamera.target.y, SessionData::minCameraFocusY, SessionData::maxCameraFocusY);
    } else {
        SessionData::gameCamera.target.y = 0.0f;
    }
}
void drawScaledScreen() {
    Rectangle destination = { 0.0f, 0.0f, (float)SessionData::windowWidth, (float)SessionData::windowHeight };
    Rectangle source = { SessionData::effectiveSource.x, SessionData::effectiveSource.y, SessionData::effectiveSource.width, -SessionData::effectiveSource.height };
    DrawTexturePro(SessionData::target.texture, source, destination, { 0,0 }, 0.0f, WHITE);
}
void displayDebugInfo() {
    DrawFPS(10, 10);
    DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", SessionData::windowWidth, SessionData::windowHeight, SessionData::windowAspectRatio), 10, 30, 20, LIME);
    DrawText(TextFormat("Effective Src: pos (%.0f,%.0f) %.0fx%.0f",
        SessionData::effectiveSource.x,
        SessionData::effectiveSource.y,
        SessionData::effectiveSource.width,
        SessionData::effectiveSource.height),
        10, 55, 20, LIME
    );
    DrawText(TextFormat("Player Pos: %.1f, %.1f", player.position.x, player.position.y), 10, 80, 20, BLACK);
    DrawText(TextFormat("Command: %d", SessionData::command), 10, 105, 20, BLACK);
    DrawText(TextFormat("Camera Zoom: %.2fx", SessionData::gameCamera.zoom), 10, 130, 20, BLACK);
    DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 155, 20, BLACK);
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