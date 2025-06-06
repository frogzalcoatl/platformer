#include "sahars_platformer.h"
#include <algorithm>

void keyCommands(int command) {
    command = 0;
    if (IsKeyDown(KEY_W)) command |= 1;
    if (IsKeyDown(KEY_S)) command |= 2;
    if (IsKeyDown(KEY_A)) command |= 4;
    if (IsKeyDown(KEY_D)) command |= 8;
    if (IsKeyDown(KEY_R)) command |= 16;
    if (IsKeyDown(KEY_V)) command |= 32;
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) command |= 64;
    if (command & 1) {
        if (GameConfig::ENABLE_GRAVITY) {
            if (player.isGrounded) {
                player.velocity.y = GameConfig::JUMP_VELOCITY;
                player.isGrounded = false;
            }
        } else {
            player.velocity.y += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
        }
    }
    if (command & 2) player.velocity.y -= GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (command & 4) player.velocity.x -= GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (command & 8) player.velocity.x += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (!(command & 8) && !(command & 4) && player.isGrounded) {
        float dampingDivisor = 1.0f + GameConfig::GROUND_FRICTION * SessionData::deltaTime;
        if (dampingDivisor > 0.0001) player.velocity.x /= dampingDivisor;
    }
    if (command & 16) {
        player.moveTo(GameConfig::PLAYER_SPAWN_POINT.x, GameConfig::PLAYER_SPAWN_POINT.y);
        SessionData::camera.target = player.position;
    }
    if (command & 32) {
        player.velocity = { 0, 0 };
        if (GameConfig::ENABLE_GRAVITY) player.applyGravity({ player.position.x, 1000.0f }, GameConfig::GRAVITY_PULL_FACTOR);
    }
    if (command & 64 && IsKeyPressed(KEY_ZERO)) SessionData::camera.zoom = 1.0f;
    if (IsKeyPressed(KEY_LEFT_SHIFT)) GameConfig::cameraShouldFollow = !GameConfig::cameraShouldFollow;
    if (IsKeyPressed(KEY_F3)) GameConfig::showDebugInfo = !GameConfig::showDebugInfo;
    if (IsKeyPressed(KEY_F11)) platformerFullscreenToggle();
    if (IsKeyPressed(KEY_G)) GameConfig::ENABLE_GRAVITY = !GameConfig::ENABLE_GRAVITY;
    Vector2 mouseWheelMove = GetMouseWheelMoveV();
    if (mouseWheelMove.y != 0.0f) {
        SessionData::camera.zoom += mouseWheelMove.y / 25.0f;
        SessionData::camera.zoom = std::clamp(SessionData::camera.zoom, GameConfig::MIN_CAMERA_ZOOM, GameConfig::MAX_CAMERA_ZOOM);
    }
}