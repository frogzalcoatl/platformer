#include "sahars_platformer.h"
#include <cmath>
#include <algorithm>

Entity::Entity(float xPosition, float yPosition, float _width, float _height, Color _color, bool _isStatic) {
    position = { xPosition, yPosition };
    velocity = { 0.0f, 0.0f };
    width = _width;
    height = _height;
    aabb = getBox(position, width, height);
    color = _color;
    isStatic = _isStatic;
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
    applyGravity({ position.x, -GameConfig::GROUND_HEIGHT }, GameConfig::GRAVITY_PULL_FACTOR);
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
}
void Entity::draw() const {
    DrawRectangleV({ aabb.min.x * SessionData::pixelsPerBlock, aabb.min.y * SessionData::pixelsPerBlock }, { width * SessionData::pixelsPerBlock, height * SessionData::pixelsPerBlock }, color);
    //DrawRectangleLinesEx(AABBtoRectangle(aabb), 1.0f, RED);
}
static const float GROUND_OBSTACLE_WIDTH = 4.0f;
void newGroundObstacle(float x, float height, Color color) {
    entities.push_back(Entity(x, height / 2.0f + GameConfig::GROUND_HEIGHT, GROUND_OBSTACLE_WIDTH, height, color));
}
void spawnTestEntities() {
    player.moveTo(GameConfig::PLAYER_SPAWN_POINT.x, GameConfig::PLAYER_SPAWN_POINT.y);
    entities.push_back(Entity(0.0f, 0.0, GameConfig::GROUND_WIDTH, GameConfig::GROUND_HEIGHT * 2, DARKGREEN));
    newGroundObstacle(25.0f, 6.0f);
    newGroundObstacle(-25.0f, 6.0f);
    newGroundObstacle(50.0f, 12.0f);
    newGroundObstacle(-50.0f, 12.0f);
    newGroundObstacle(80.0f, 9.0f);
    newGroundObstacle(-80.0f, 9.0f);
    newGroundObstacle(-GameConfig::GROUND_WIDTH / 2.0f - (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    newGroundObstacle(GameConfig::GROUND_WIDTH / 2.0f + (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    const float entityWidth = 10.0f;
    const float entityHeight = 10.0f;
    for (size_t i = 0; i < GameConfig::DUMMY_ENTITY_COUNT; i++) {
        entities.push_back(Entity(Random::getFloat(-GameConfig::GROUND_WIDTH / 2.0f, GameConfig::GROUND_WIDTH / 2.0f), Random::getFloat(0, 50), 1.0f, 1.0f, YELLOW, false));
    }
}