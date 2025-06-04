#include "sahars_platformer.h"

void keyCommands() {
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
        player.velocity.y = -GameConfig::JUMP_VELOCITY;
        player.isGrounded = false;
    }
    if (SessionData::command & 2) player.velocity.y += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (SessionData::command & 4) player.velocity.x -= GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (SessionData::command & 8) player.velocity.x += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (!(SessionData::command & 8) && !(SessionData::command & 4) && player.isGrounded) {
        float dampingDivisor = 1.0f + GameConfig::GROUND_FRICTION * SessionData::deltaTime;
        if (dampingDivisor > 0.0001) player.velocity.x /= dampingDivisor;
    }
    if (SessionData::command & 16) {
        player.moveTo(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - player.height / 2.0f);
        GameConfig::cameraFocus = player.position;
    }
    if (SessionData::command & 32) {
        player.velocity = { 0, 0 };
        player.applyGravity({ player.position.x, GameConfig::SOURCE_HEIGHT }, -GameConfig::GRAVITY_PULL_FACTOR);
    }
}