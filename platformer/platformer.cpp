#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <format>

struct Box {
    Vector2 min;
    Vector2 max;
};

namespace GameConfig {
    const float WINDOW_WIDTH = 800.0f;
    const float WINDOW_HEIGHT = 600.0f;
    const int GROUND_WIDTH = WINDOW_WIDTH * 4;
    const int GROUND_HEIGHT = 20;
    const float GRAVITY_STRENGTH = 0.15f;
    const float JUMP_STRENGTH = -10.0f;
    const float DEAD_ZONE_PERCENT_X = 0.3f;
    const float DEAD_ZONE_PERCENT_Y = 0.4f;
    const float MIN_CAMERA_FOCUS_X = -GROUND_WIDTH / 2 + (WINDOW_WIDTH / 2.0f);
    const float MAX_CAMERA_FOCUS_X = -MIN_CAMERA_FOCUS_X;
    const float MIN_CAMERA_FOCUS_Y = WINDOW_HEIGHT / 2.0f;
    const float MAX_CAMERA_FOCUS_Y = WINDOW_HEIGHT / 2.0f;
    bool showDebugInfo = true;
    bool cameraShouldFollow = true;
    bool prevCameraFollowState = true;
    Vector2 cameraFocus = { 0.0f, 0.0f };
}

Box getBox(Vector2 position, float width, float height) {
    return { { position.x - width / 2, position.y - height / 2 }, { position.x + width / 2, position.y + height / 2 } };
};

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    Box aabb;
    Color color;
    bool isStatic;
    bool isGrounded;
    Entity(float xPosition, float yPosition, float entityWidth, float entityHeight, Color entityColor, bool isEntityStatic = true) {
        position = { xPosition, yPosition };
        velocity = { 0, 0 };
        width = entityWidth;
        height = entityHeight;
        aabb = getBox(position, width, height);
        color = entityColor;
        isStatic = isEntityStatic;
        isGrounded = false;
    };
    void updateAABB() {
        aabb = getBox(position, width, height);
    }
    void moveBy(float x, float y) {
        position.x += x;
        position.y += y;
        updateAABB();
    };
    void moveTo(float x, float y) {
        position.x = x;
        position.y = y;
        velocity.x = 0;
        velocity.y = 0;
        updateAABB();
    };
    void applyGravity(Vector2 gravityCenter, float pullFactor) {
        if (isStatic) return;
        float distanceX = gravityCenter.x - position.x;
        float distanceY = gravityCenter.y - position.y;
        float distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));
        if (distance == 0) distance = 1;
        velocity.x += (distanceX / distance) * pullFactor;
        velocity.y += (distanceY / distance) * pullFactor;
    };
    void tick() {
        velocity.x = std::clamp(velocity.x, -5.0f, 5.0f);
        velocity.y = std::clamp(velocity.y, -15.0f, 15.0f);
        position.x += velocity.x;
        position.y += velocity.y;
        updateAABB();
    };
    void draw() {
        float drawX, drawY;
        if (GameConfig::cameraShouldFollow) {
            drawX = aabb.min.x - GameConfig::cameraFocus.x + (GameConfig::WINDOW_WIDTH / 2);
            drawY = aabb.min.y - GameConfig::cameraFocus.y + (GameConfig::WINDOW_HEIGHT / 2);
        }
        else {
            drawX = aabb.min.x + (GameConfig::WINDOW_WIDTH / 2);
            drawY = aabb.min.y;
        }
        DrawRectangle(drawX, drawY, width, height, color);
        //DrawRectangleLines(aabb.min.x + offsetX, aabb.min.y + offsetY, aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y, RED);
    };
};

std::vector<Entity> platforms = {};
std::vector<Entity> entities = {};
Entity player = Entity(0, GameConfig::WINDOW_HEIGHT - 30, 20, 20, DARKPURPLE, false);

void newGroundObsticle(float x, float height, Color color = LIGHTGRAY) {
    platforms.push_back(Entity(x, GameConfig::WINDOW_HEIGHT - GameConfig::GROUND_HEIGHT - height / 2, 50, height, color));
}

void Collision(Entity& entity) {
    if (entity.isStatic) return;
    entity.isGrounded = false;
    for (size_t i = 0; i < platforms.size(); i++) {
        if (!(entity.aabb.max.x > platforms[i].aabb.min.x &&
            entity.aabb.min.x < platforms[i].aabb.max.x &&
            entity.aabb.max.y > platforms[i].aabb.min.y &&
            entity.aabb.min.y < platforms[i].aabb.max.y)
            ) continue;
        float overlapX_rightPush = entity.aabb.max.x - platforms[i].aabb.min.x;
        float overlapX_leftPush = platforms[i].aabb.max.x - entity.aabb.min.x;
        float overlapY_bottomPush = entity.aabb.max.y - platforms[i].aabb.min.y;
        float overlapY_topPush = platforms[i].aabb.max.y - entity.aabb.min.y;
        float minOverlapX = std::fmin(overlapX_rightPush, overlapX_leftPush);
        float minOverlapY = std::fmin(overlapY_bottomPush, overlapY_topPush);
        const float penetration_threshold = 0.01f;
        if (minOverlapX < minOverlapY && minOverlapX > penetration_threshold) {
            entity.velocity.x = 0;
            float displacementX;
            if (overlapX_rightPush < overlapX_leftPush) {
                displacementX = -overlapX_rightPush;
            } else {
                displacementX = overlapX_leftPush;
            }
            entity.position.x += displacementX;
        } else if (minOverlapY <= minOverlapX && minOverlapY > penetration_threshold) {
            entity.velocity.y = 0;
            float displacementY;
            if (overlapY_bottomPush < overlapY_topPush) {
                displacementY = -overlapY_bottomPush;
                entity.isGrounded = true;
            }
            else {
                displacementY = overlapY_topPush;
            }
            entity.position.y += displacementY;
        }
    }
    entity.updateAABB();
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, "Platformer");
    SetTargetFPS(GetMonitorRefreshRate(0));
    GameConfig::cameraFocus = player.position;
    platforms.push_back(Entity(0, GameConfig::WINDOW_HEIGHT - GameConfig::GROUND_HEIGHT / 2, GameConfig::GROUND_WIDTH, GameConfig::GROUND_HEIGHT, DARKGREEN));
    newGroundObsticle(400, 100);
    newGroundObsticle(-400, 50);
    newGroundObsticle(820, 200);
    newGroundObsticle(-820, 200);
    newGroundObsticle(1275, 150);
    newGroundObsticle(-1275, 150);
    newGroundObsticle(-1625, 600, SKYBLUE);
    newGroundObsticle(1625, 600, SKYBLUE);
    const int entityWidth = 10;
    const int entityHeight = 10;
    for (size_t i = 0; i < 0; i++) {
        entities.push_back(Entity(dis(gen) * GameConfig::WINDOW_HEIGHT, dis(gen) * GameConfig::WINDOW_WIDTH, 10, 10, YELLOW));
    }
    while (!WindowShouldClose()) {
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
        if (command & 8) player.velocity.x += 0.1f;
        if (command & 16) player.moveTo(0, GameConfig::WINDOW_HEIGHT - GameConfig::GROUND_HEIGHT);
        if (command & 32) player.velocity = { 0, 0 };
        if (GameConfig::cameraShouldFollow && !GameConfig::prevCameraFollowState) {
            GameConfig::cameraFocus = player.position;
        }
        GameConfig::prevCameraFollowState = GameConfig::cameraShouldFollow;
        if (GameConfig::cameraShouldFollow) {
            float deadZoneWidth = GameConfig::WINDOW_WIDTH * GameConfig::DEAD_ZONE_PERCENT_X;
            float deadZoneHeight = GameConfig::WINDOW_HEIGHT * GameConfig::DEAD_ZONE_PERCENT_Y;
            float deadZoneScreenMinX = (GameConfig::WINDOW_WIDTH - deadZoneWidth) / 2;
            float deadZoneScreenMaxX = deadZoneScreenMinX + deadZoneWidth;
            float deadZoneScreenMinY = (GameConfig::WINDOW_HEIGHT - deadZoneHeight) / 2;
            float deadZoneScreenMaxY = deadZoneScreenMinY + deadZoneHeight;
            float playerScreenX = player.position.x - GameConfig::cameraFocus.x + (GameConfig::WINDOW_WIDTH / 2);
            float playerScreenY = player.position.y - GameConfig::cameraFocus.y + (GameConfig::WINDOW_HEIGHT / 2);
            if (playerScreenX < deadZoneScreenMinX) {
                GameConfig::cameraFocus.x = player.position.x - deadZoneScreenMinX + (GameConfig::WINDOW_WIDTH / 2);
            } else if (playerScreenX > deadZoneScreenMaxX) {
                GameConfig::cameraFocus.x = player.position.x - deadZoneScreenMaxX + (GameConfig::WINDOW_WIDTH / 2);
            }
            if (playerScreenY < deadZoneScreenMinY) {
                GameConfig::cameraFocus.y = player.position.y - deadZoneScreenMinY + (GameConfig::WINDOW_WIDTH / 2);
            } else if (playerScreenY > deadZoneScreenMaxY) {
                GameConfig::cameraFocus.y = player.position.y - deadZoneScreenMaxY + (GameConfig::WINDOW_WIDTH  / 2);
            }
        }
        GameConfig::cameraFocus.x = std::clamp(GameConfig::cameraFocus.x, GameConfig::MIN_CAMERA_FOCUS_X, GameConfig::MAX_CAMERA_FOCUS_X);
        GameConfig::cameraFocus.y = std::clamp(GameConfig::cameraFocus.y, GameConfig::MIN_CAMERA_FOCUS_Y, GameConfig::MAX_CAMERA_FOCUS_Y);
        BeginDrawing();
        ClearBackground(SKYBLUE);
        for (size_t i = 0; i < platforms.size(); i++) {
            platforms[i].draw();
        }
        for (size_t i = 0; i < entities.size(); i++) {
            entities[i].applyGravity({ entities[i].position.x, 600 }, GameConfig::GRAVITY_STRENGTH);
            Collision(entities[i]);
            entities[i].tick();
            entities[i].draw();
        }
        player.applyGravity({ player.position.x, 600.0f }, GameConfig::GRAVITY_STRENGTH);
        Collision(player);
        player.tick();
        player.draw();
        if (GameConfig::showDebugInfo) {
            DrawFPS(10, 10);
            DrawText(TextFormat("Player Pos: %.1f, %.1f", player.position.x, player.position.y), 10, 30, 10, BLACK);
            DrawText(TextFormat("Command: %d", command), 10, 50, 10, BLACK);
            DrawText(GameConfig::cameraShouldFollow ? "Camera: FOLLOW" : "Camera: STATIC", 10, 70, 10, BLACK);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}