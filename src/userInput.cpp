#include "sahars_platformer.h"
#include <algorithm>

void userInput() {
    SessionData::leftClickDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    SessionData::rightClickDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    SessionData::mouseWheelMove = GetMouseWheelMove();
    SessionData::mousePosition = GetMousePosition();
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (SessionData::uiScreen == 0) {
            SessionData::uiScreen = 1;
        } else {
            SessionData::uiScreen--;
        }
    }
    if (IsKeyPressed(KEY_F3)) SessionData::showDebugInfo = !SessionData::showDebugInfo;
    if (IsKeyPressed(KEY_F11)) platformerFullscreenToggle();
    if (SessionData::uiScreen == 0) gameplayInput();
}
void scrollZoom() {
    if (SessionData::mouseWheelMove == 0.0f) return;
    if (SessionData::cameraShouldFollow) {
        SessionData::userZoom += SessionData::mouseWheelMove / 15.0f;
        SessionData::userZoom = Clamp(SessionData::userZoom, GameConfig::MIN_USER_ZOOM, GameConfig::MAX_USER_ZOOM);
    } else {
        Vector2 mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), SessionData::camera);
        SessionData::camera.offset = GetMousePosition();
        SessionData::camera.target = mouseWorldPosition;
        SessionData::userZoom += SessionData::mouseWheelMove / 15.0f;
        SessionData::userZoom = Clamp(SessionData::userZoom, GameConfig::MIN_USER_ZOOM, GameConfig::MAX_USER_ZOOM);
    }
}
void gameplayInput() {
    if (IsKeyDown(KEY_W)) {
        if (SessionData::isGravityEnabled) {
            if (player.isGrounded) {
                player.velocity.y = GameConfig::JUMP_VELOCITY;
                player.isGrounded = false;
            }
        } else {
            player.velocity.y += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
        }
    }
    if (IsKeyDown(KEY_S)) player.velocity.y -= GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (IsKeyDown(KEY_A)) player.velocity.x -= GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (IsKeyDown(KEY_D)) player.velocity.x += GameConfig::PLAYER_ACCELERATION * SessionData::deltaTime;
    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && player.isGrounded) {
        float dampingDivisor = 1.0f + GameConfig::GROUND_FRICTION * SessionData::deltaTime;
        if (dampingDivisor > 0.0001) player.velocity.x /= dampingDivisor;
    }
    if (IsKeyDown(KEY_R)) {
        player.moveTo(GameConfig::PLAYER_SPAWN_POINT.x, GameConfig::PLAYER_SPAWN_POINT.y);
        if (SessionData::cameraShouldFollow) {
            SessionData::camera.target = player.position;
            SessionData::userZoom = 1.0f;
        }
    }
    if (IsKeyDown(KEY_V)) {
        player.velocity = { 0, 0 };
        if (SessionData::isGravityEnabled) player.applyGravity({ player.position.x, 1000.0f }, GameConfig::GRAVITY_PULL_FACTOR);
    }
    if (IsKeyPressed(KEY_G)) SessionData::isGravityEnabled = !SessionData::isGravityEnabled;
    bool isCtrlDown = false;
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) isCtrlDown = true;
    if (isCtrlDown && IsKeyPressed(KEY_ZERO)) SessionData::userZoom = 1.0f;
    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
        if (!SessionData::cameraShouldFollow) {
            SessionData::userZoom = 1.0f;
        }
        SessionData::cameraShouldFollow = !SessionData::cameraShouldFollow;
    }
    scrollZoom();
}