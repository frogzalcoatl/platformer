#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <random>

static void setup() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    MaximizeWindow();
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    SessionData::currentMonitor = GetCurrentMonitor();
    SessionData::monitorRefreshRate = GetMonitorRefreshRate(SessionData::currentMonitor);
    SetTargetFPS(SessionData::monitorRefreshRate);
    SessionData::monitorAspectRatio = GetMonitorWidth(SessionData::currentMonitor) / GetMonitorHeight(SessionData::currentMonitor);
    spawnTestEntities();
    player.moveTo(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
    GameConfig::cameraFocus = player.position;
    SessionData::gameCamera.offset = { GameConfig::SOURCE_WIDTH / 2.0f, GameConfig::SOURCE_HEIGHT / 2.0f };
    SessionData::gameCamera.rotation = 0.0f;
    SessionData::gameCamera.zoom = 1.0f;
    SessionData::target = LoadRenderTexture(GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT);
}
static void monitorAndWindowChecks() {
    if (GetCurrentMonitor() != SessionData::currentMonitor) {
        SessionData::currentMonitor = GetCurrentMonitor();
        SessionData::monitorAspectRatio = GetMonitorWidth(SessionData::currentMonitor) / GetMonitorHeight(SessionData::currentMonitor);
    }
    if (GetMonitorRefreshRate(SessionData::currentMonitor) != SessionData::monitorRefreshRate) {
        SessionData::monitorRefreshRate = GetMonitorRefreshRate(SessionData::currentMonitor);
        SetTargetFPS(SessionData::monitorRefreshRate);
    }
    if (IsWindowResized()) {
        SessionData::windowWidth = GetScreenWidth();
        SessionData::windowHeight = GetScreenHeight();
        SessionData::windowAspectRatio = (float)SessionData::windowWidth / (float)SessionData::windowHeight;
        SessionData::display = { 0.0f, 0.0f, (float)SessionData::windowWidth, (float)SessionData::windowHeight };
    }
}
static void keyCommands() {
    SessionData::command = 0;
    if (IsKeyDown(KEY_W)) SessionData::command |= 1;
    if (IsKeyDown(KEY_S)) SessionData::command |= 2;
    if (IsKeyDown(KEY_A)) SessionData::command |= 4;
    if (IsKeyDown(KEY_D)) SessionData::command |= 8;
    if (IsKeyDown(KEY_R)) SessionData::command |= 16;
    if (IsKeyDown(KEY_V)) SessionData::command |= 32;
    if (IsKeyPressed(KEY_LEFT_SHIFT)) GameConfig::cameraShouldFollow = !GameConfig::cameraShouldFollow;
    if (IsKeyPressed(KEY_F3)) GameConfig::showDebugInfo = !GameConfig::showDebugInfo;
    if (SessionData::command & 1 && player.isGrounded) {
        player.velocity.y = GameConfig::JUMP_STRENGTH;
        player.isGrounded = false;
    }
    if (SessionData::command & 2) player.velocity.y += 0.2f;
    if (SessionData::command & 4) player.velocity.x -= 0.2f;
    if (SessionData::command & 8) player.velocity.x += 0.2f;
    if (SessionData::command & 16) player.moveTo(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
    if (SessionData::command & 32) player.velocity = { 0, 0 };
}
static void cameraFocus() {
    if (GameConfig::cameraShouldFollow && !GameConfig::prevCameraFollowState) {
        GameConfig::cameraFocus = player.position;
    }
    GameConfig::prevCameraFollowState = GameConfig::cameraShouldFollow;
    if (!GameConfig::cameraShouldFollow) return;
    float visibleWorldWidth = SessionData::effectiveRenderTextureSource.width / SessionData::gameCamera.zoom;
    float visibleWorldHeight = SessionData::effectiveRenderTextureSource.height / SessionData::gameCamera.zoom;
    SessionData::minCameraFocusX = -GameConfig::GROUND_WIDTH / 2.0f + (visibleWorldWidth / 2.0f);
    SessionData::maxCameraFocusX = GameConfig::GROUND_WIDTH / 2.0f - (visibleWorldWidth / 2.0f);
    SessionData::minCameraFocusY = GameConfig::SOURCE_HEIGHT / 2.0f;
    SessionData::maxCameraFocusY = SessionData::minCameraFocusY;
    float playerRelativeX = player.position.x - GameConfig::cameraFocus.x;
    float playerRelativeY = player.position.y - GameConfig::cameraFocus.y;
    float deadZoneWorldLimitX = (visibleWorldWidth * (GameConfig::DEAD_ZONE_PERCENT_X)) / 2.0f;
    float deadZoneWorldLimitY = (visibleWorldHeight * (GameConfig::DEAD_ZONE_PERCENT_Y)) / 2.0f;
    if (playerRelativeX > deadZoneWorldLimitX) {
        GameConfig::cameraFocus.x += playerRelativeX - deadZoneWorldLimitX;
    } else if (playerRelativeX < -deadZoneWorldLimitX) {
        GameConfig::cameraFocus.x += playerRelativeX + deadZoneWorldLimitX;
    }
    if (playerRelativeY > deadZoneWorldLimitY) {
        GameConfig::cameraFocus.y += playerRelativeY - deadZoneWorldLimitY;
    } else if (playerRelativeY < -deadZoneWorldLimitY) {
        GameConfig::cameraFocus.y += playerRelativeY + deadZoneWorldLimitY;
    }
    GameConfig::cameraFocus.x = std::clamp(GameConfig::cameraFocus.x, SessionData::minCameraFocusX, SessionData::maxCameraFocusX);
    GameConfig::cameraFocus.y = std::clamp(GameConfig::cameraFocus.y, SessionData::minCameraFocusY, SessionData::maxCameraFocusY);
    SessionData::gameCamera.target = GameConfig::cameraFocus;
}
static void updateEffectiveRenderTextureSource() {
    SessionData::effectiveRenderTextureSource = { 0, 0, GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT };
    if (SessionData::windowAspectRatio > GameConfig::SOURCE_ASPECT_RATIO) {
        SessionData::effectiveRenderTextureSource.height = GameConfig::SOURCE_WIDTH / SessionData::windowAspectRatio;
        SessionData::effectiveRenderTextureSource.y = (GameConfig::SOURCE_HEIGHT - SessionData::effectiveRenderTextureSource.height) / 2.0f;
    } else if (SessionData::windowAspectRatio < GameConfig::SOURCE_ASPECT_RATIO) {
        SessionData::effectiveRenderTextureSource.width = GameConfig::SOURCE_HEIGHT * SessionData::windowAspectRatio;
        SessionData::effectiveRenderTextureSource.x = (GameConfig::SOURCE_WIDTH - SessionData::effectiveRenderTextureSource.width) / 2.0f;
    }
}
void displayDebugInfo() {
    DrawFPS(10, 10);
    DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", SessionData::windowWidth, SessionData::windowHeight, SessionData::windowAspectRatio), 10, 30, 20, LIME);
    DrawText(TextFormat("Eff RT Src: (%.0f,%.0f) %.0fx%.0f",
        SessionData::effectiveRenderTextureSource.x,
        SessionData::effectiveRenderTextureSource.y,
        SessionData::effectiveRenderTextureSource.width,
        SessionData::effectiveRenderTextureSource.height),
        10, 55, 20, LIME
    );
    DrawText(TextFormat("Player Pos: %.1f, %.1f", player.position.x, player.position.y), 10, 80, 20, BLACK);
    DrawText(TextFormat("Command: %d", SessionData::command), 10, 105, 20, BLACK);
    DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 130, 20, BLACK);
}
int main() {
    setup();
    while (!WindowShouldClose()) {
        keyCommands();
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].tick();
        for (size_t i = 0; i < entities.size(); i++) entities[i].tick();
        player.tick();
        monitorAndWindowChecks();
        updateEffectiveRenderTextureSource();
        cameraFocus();
        BeginTextureMode(SessionData::target);
        ClearBackground(SKYBLUE);
        BeginMode2D(SessionData::gameCamera);
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].draw();
        for (size_t i = 0; i < entities.size(); i++) entities[i].draw();
        player.draw();
        EndMode2D();
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        if (SessionData::windowAspectRatio > GameConfig::SOURCE_ASPECT_RATIO) {
            SessionData::source.height = GameConfig::SOURCE_WIDTH / SessionData::windowAspectRatio;
            SessionData::source.y = (GameConfig::SOURCE_HEIGHT - SessionData::source.height) / 2.0f;
        } else if (SessionData::windowAspectRatio < GameConfig::SOURCE_ASPECT_RATIO) {
            SessionData::source.width = GameConfig::SOURCE_HEIGHT * SessionData::windowAspectRatio;
            SessionData::source.x = (GameConfig::SOURCE_WIDTH - SessionData::source.width) / 2.0f;
        }
        Rectangle destination = { 0.0f, 0.0f, (float)SessionData::windowWidth, (float)SessionData::windowHeight };
        Rectangle source = { SessionData::effectiveRenderTextureSource.x, SessionData::effectiveRenderTextureSource.y, SessionData::effectiveRenderTextureSource.width, -SessionData::effectiveRenderTextureSource.height };
        DrawTexturePro(SessionData::target.texture, source, destination, { 0,0 }, 0.0f, WHITE);
        if (GameConfig::showDebugInfo) displayDebugInfo();
        EndDrawing();
    }
    UnloadRenderTexture(SessionData::target);
    CloseWindow();
    return 0;
}