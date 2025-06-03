#include "sahars_platformer.h"
#define RAYGUI_IMPLEMENTATION
#include <random>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> keepRunning(true);
void runInterval(int (*f)(), int ms) {
    while (keepRunning) {
        (*f)();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT, "Platformer");
    MaximizeWindow();
    SetWindowMinSize(GameConfig::MIN_WINDOW_WIDTH, GameConfig::MIN_WINDOW_HEIGHT);
    SessionData::currentMonitor = GetCurrentMonitor();
    SessionData::monitorRefreshRate = GetMonitorRefreshRate(SessionData::currentMonitor);
    SetTargetFPS(SessionData::monitorRefreshRate);
    RenderTexture2D target = LoadRenderTexture(GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT);
    Camera2D gameCamera = { 0 };
    gameCamera.offset = { GameConfig::SOURCE_WIDTH / 2.0f, GameConfig::SOURCE_HEIGHT / 2.0f };
    gameCamera.rotation = 0.0f;
    gameCamera.zoom = 1.0f;
    platforms.push_back(Entity(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT / 2, GameConfig::GROUND_WIDTH, GameConfig::GROUND_HEIGHT, DARKGREEN));
    newGroundObstacle(400, 100);
    newGroundObstacle(-400, 50);
    newGroundObstacle(820, 200);
    newGroundObstacle(-820, 200);
    newGroundObstacle(1275, 150);
    newGroundObstacle(-1275, 150);
    newGroundObstacle(-GameConfig::GROUND_WIDTH / 2, 600, SKYBLUE);
    newGroundObstacle(GameConfig::GROUND_WIDTH / 2, 600, SKYBLUE);
    player.moveTo(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
    GameConfig::cameraFocus = player.position;
    const int entityWidth = 10;
    const int entityHeight = 10;
    for (size_t i = 0; i < 0; i++) {
        entities.push_back(Entity(dis(gen) * GameConfig::SOURCE_HEIGHT, dis(gen) * GameConfig::SOURCE_WIDTH, 10, 10, YELLOW));
    }
    while (!WindowShouldClose()) {
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
        }
        int command = 0;
        if (IsKeyDown(KEY_W)) command |= 1;
        if (IsKeyDown(KEY_S)) command |= 2;
        if (IsKeyDown(KEY_A)) command |= 4;
        if (IsKeyDown(KEY_D)) command |= 8;
        if (IsKeyDown(KEY_R)) command |= 16;
        if (IsKeyDown(KEY_V)) command |= 32;
        if (IsKeyPressed(KEY_LEFT_SHIFT)) GameConfig::cameraShouldFollow = !GameConfig::cameraShouldFollow;
        if (IsKeyPressed(KEY_F3)) GameConfig::showDebugInfo = !GameConfig::showDebugInfo;
        if (command & 1 && player.isGrounded) {
            player.velocity.y = GameConfig::JUMP_STRENGTH;
            player.isGrounded = false;
        }
        if (command & 2) player.velocity.y += 0.2f;
        if (command & 4) player.velocity.x -= 0.2f;
        if (command & 8) player.velocity.x += 0.2f;
        if (command & 16) player.moveTo(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
        if (command & 32) player.velocity = { 0, 0 };
        if (GameConfig::cameraShouldFollow && !GameConfig::prevCameraFollowState) {
            GameConfig::cameraFocus = player.position;
        }
        GameConfig::prevCameraFollowState = GameConfig::cameraShouldFollow;
        if (GameConfig::cameraShouldFollow) {
            float deadZoneWidth = GameConfig::SOURCE_WIDTH * GameConfig::DEAD_ZONE_PERCENT_X;
            float deadZoneHeight = GameConfig::SOURCE_HEIGHT * GameConfig::DEAD_ZONE_PERCENT_Y;
            float deadZoneScreenMinX = (GameConfig::SOURCE_WIDTH - deadZoneWidth) / 2;
            float deadZoneScreenMaxX = deadZoneScreenMinX + deadZoneWidth;
            float deadZoneScreenMinY = (GameConfig::SOURCE_HEIGHT - deadZoneHeight) / 2;
            float deadZoneScreenMaxY = deadZoneScreenMinY + deadZoneHeight;
            float playerScreenX = player.position.x - GameConfig::cameraFocus.x + (GameConfig::SOURCE_WIDTH / 2);
            float playerScreenY = player.position.y - GameConfig::cameraFocus.y + (GameConfig::SOURCE_HEIGHT / 2);
            if (playerScreenX < deadZoneScreenMinX) {
                GameConfig::cameraFocus.x = player.position.x - deadZoneScreenMinX + (GameConfig::SOURCE_WIDTH / 2);
            } else if (playerScreenX > deadZoneScreenMaxX) {
                GameConfig::cameraFocus.x = player.position.x - deadZoneScreenMaxX + (GameConfig::SOURCE_WIDTH / 2);
            }
            if (playerScreenY < deadZoneScreenMinY) {
                GameConfig::cameraFocus.y = player.position.y - deadZoneScreenMinY + (GameConfig::SOURCE_HEIGHT / 2);
            } else if (playerScreenY > deadZoneScreenMaxY) {
                GameConfig::cameraFocus.y = player.position.y - deadZoneScreenMaxY + (GameConfig::SOURCE_HEIGHT / 2);
            }
        }
        GameConfig::cameraFocus.x = std::clamp(GameConfig::cameraFocus.x, GameConfig::MIN_CAMERA_FOCUS_X, GameConfig::MAX_CAMERA_FOCUS_X);
        GameConfig::cameraFocus.y = std::clamp(GameConfig::cameraFocus.y, GameConfig::MIN_CAMERA_FOCUS_Y, GameConfig::MAX_CAMERA_FOCUS_Y);
        for (size_t i = 0; i < entities.size(); i++) {
            entities[i].applyGravity({ entities[i].position.x, GameConfig::SOURCE_HEIGHT }, GameConfig::GRAVITY_STRENGTH);
            collision(entities[i]);
            entities[i].tick();
        }
        player.applyGravity({ player.position.x, GameConfig::SOURCE_HEIGHT }, GameConfig::GRAVITY_STRENGTH);
        collision(player);
        player.tick();
        gameCamera.target = GameConfig::cameraFocus;
        BeginTextureMode(target);
        ClearBackground(SKYBLUE);
        BeginMode2D(gameCamera);
        for (size_t i = 0; i < platforms.size(); i++) platforms[i].draw();
        for (size_t i = 0; i < entities.size(); i++) entities[i].draw();
        player.draw();
        EndMode2D();
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();
        float windowAspectRatio = screenWidth / screenHeight;
        Rectangle source = { 0.0f, 0.0f, GameConfig::SOURCE_WIDTH, GameConfig::SOURCE_HEIGHT };
        Rectangle display = { 0.0f, 0.0f, screenWidth, screenHeight };
        if (windowAspectRatio > GameConfig::SOURCE_ASPECT_RATIO) {
            source.height = GameConfig::SOURCE_WIDTH / windowAspectRatio;
            source.y = (GameConfig::SOURCE_HEIGHT - source.height) / 2.0f;
        } else if (windowAspectRatio < GameConfig::SOURCE_ASPECT_RATIO) {
            source.width = GameConfig::SOURCE_HEIGHT * windowAspectRatio;
            source.x = (GameConfig::SOURCE_WIDTH - source.width) / 2.0f;
        }
        source.height *= -1;
        DrawTexturePro(target.texture, source, display, { 0,0 }, 0.0f, WHITE);
        if (GameConfig::showDebugInfo) {
            DrawFPS(10, 10);
            DrawText(TextFormat("Win:%.0fx%.0f (AR: %.2f)", screenWidth, screenHeight, windowAspectRatio), 10, 30, 20, LIME);
            DrawText(TextFormat("Src: (%.0f,%.0f) %.0fx%.0f", source.x, fabsf(source.y + source.height), fabsf(source.width), fabsf(source.height)), 10, 55, 20, LIME);
            DrawText(TextFormat("Player Pos: %.1f, %.1f", player.position.x, player.position.y), 10, 80, 20, BLACK);
            DrawText(TextFormat("Command: %d", command), 10, 105, 20, BLACK);
            DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 130, 20, BLACK);
        }
        EndDrawing();
    }
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}