#include "sahars_platformer.h"
#include <cmath>
#include <algorithm>

Entity::Entity(float xPosition, float yPosition, float entityWidth, float entityHeight, Color entityColor, bool isEntityStatic) {
    position = { xPosition, yPosition };
    velocity = { 0.0f, 0.0f };
    width = entityWidth;
    height = entityHeight;
    aabb = getBox(position, width, height);
    color = entityColor;
    isStatic = isEntityStatic;
    isGrounded = false;
};
void Entity::updateAABB() {
    aabb = getBox(position, width, height);
}
void Entity::moveBy(float x, float y) {
    position.x += x;
    position.y += y;
    updateAABB();
}
void Entity::moveTo(float x, float y) {
    position.x = x;
    position.y = y;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    updateAABB();
}
void Entity::applyGravity(Vector2 gravityCenter, float pullFactor) {
    if (isStatic) return;
    float distanceX = gravityCenter.x - position.x;
    float distanceY = gravityCenter.y - position.y;
    float distance = std::sqrt(std::powf(distanceX, 2.0f) + std::pow(distanceY, 2.0f));
    if (distance == 0.0f) distance = 1.0f;
    velocity.x += (distanceX / distance) * pullFactor * SessionData::deltaTime;
    velocity.y += (distanceY / distance) * pullFactor * SessionData::deltaTime;
}
void Entity::tick() {
    if (isStatic) return;
    applyGravity({ position.x, GameConfig::SOURCE_HEIGHT }, GameConfig::GRAVITY_PULL_FACTOR);
    if (GameConfig::AIR_FRICTION > 0.0f && SessionData::deltaTime > 0.0f) {
        float dampingDivisor = 1.0f + GameConfig::AIR_FRICTION * SessionData::deltaTime;
        if (dampingDivisor > 0.0001f) {
            velocity.x /= dampingDivisor;
            velocity.y /= dampingDivisor;
        }
    }
    velocity.x = std::clamp(velocity.x, -GameConfig::MAX_VELOCITY_X, GameConfig::MAX_VELOCITY_X);
    velocity.y = std::clamp(velocity.y, -GameConfig::MAX_VELOCITY_Y, GameConfig::MAX_VELOCITY_Y);
    position.x += velocity.x * SessionData::deltaTime;
    position.y += velocity.y * SessionData::deltaTime;
    updateAABB();
    collision(*this);
}
void Entity::draw() const {
    DrawRectangleV(aabb.min, { width, height }, color);
    //DrawRectangleLinesEx({ aabb.min.x, aabb.min.y, aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y }, 1.0f, RED);
}
static const float GROUND_OBSTACLE_WIDTH = 50.0f;
void newGroundObstacle(float x, float height, Color color) {
    platforms.push_back(Entity(x, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - height / 2.0f, GROUND_OBSTACLE_WIDTH, height, color));
}
void spawnTestEntities() {
    platforms.push_back(Entity(0.0f, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT / 2.0f, GameConfig::GROUND_WIDTH, GameConfig::GROUND_HEIGHT, DARKGREEN));
    newGroundObstacle(400.0f, 100.0f);
    newGroundObstacle(-400.0f, 100.0f);
    newGroundObstacle(820.0f, 200.0f);
    newGroundObstacle(-820.0f, 200.0f);
    newGroundObstacle(1275.0f, 150.0f);
    newGroundObstacle(-1275.0f, 150.0f);
    newGroundObstacle(-GameConfig::GROUND_WIDTH / 2.0f - (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    newGroundObstacle(GameConfig::GROUND_WIDTH / 2.0f + (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    const float entityWidth = 10.0f;
    const float entityHeight = 10.0f;
    for (size_t i = 0; i < GameConfig::DUMMY_ENTITY_COUNT; i++) {
        entities.push_back(Entity(Random::getFloat(0, GameConfig::SOURCE_WIDTH), Random::getFloat(0, GameConfig::SOURCE_HEIGHT), 10.0f, 10.0f, YELLOW));
    }
}