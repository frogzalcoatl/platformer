#include "sahars_platformer.h"
#include <cmath>
#include <algorithm>

Entity::Entity(float xPosition, float yPosition, float entityWidth, float entityHeight, Color entityColor, bool isEntityStatic) {
    position = { xPosition, yPosition };
    velocity = { 0, 0 };
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
    velocity.x = 0;
    velocity.y = 0;
    updateAABB();
}
void Entity::applyGravity(Vector2 gravityCenter, float pullFactor) {
    if (isStatic) return;
    float distanceX = gravityCenter.x - position.x;
    float distanceY = gravityCenter.y - position.y;
    float distance = std::sqrt(std::pow(distanceX, 2) + std::pow(distanceY, 2));
    if (distance == 0) distance = 1;
    velocity.x += (distanceX / distance) * pullFactor;
    velocity.y += (distanceY / distance) * pullFactor;
}
void Entity::tick() {
    if (isStatic) return;
    applyGravity({ position.x, GameConfig::SOURCE_HEIGHT }, GameConfig::GRAVITY_STRENGTH);
    collision(*this);
    velocity.x *= 0.99f;
    velocity.y *= 0.99f;
    velocity.x = std::clamp(velocity.x, -7.5f, 7.5f);
    velocity.y = std::clamp(velocity.y, -40.0f, 40.0f);
    position.x += velocity.x;
    position.y += velocity.y;
    updateAABB();
}
void Entity::draw() {
    DrawRectangle((int)aabb.min.x, (int)aabb.min.y, (int)width, (int)height, color);
    //DrawRectangleLines((int)aabb.min.x, (int)aabb.min.y, (int)(aabb.max.x - aabb.min.x), (int)(aabb.max.y - aabb.min.y), RED);
}
static const int GROUND_OBSTACLE_WIDTH = 50;
void newGroundObstacle(float x, float height, Color color) {
    platforms.push_back(Entity(x, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - height / 2, (float)GROUND_OBSTACLE_WIDTH, height, color));
}
void spawnTestEntities() {
    platforms.push_back(Entity(0, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT / 2, GameConfig::GROUND_WIDTH, GameConfig::GROUND_HEIGHT, DARKGREEN));
    newGroundObstacle(400.0f, 100.0f);
    newGroundObstacle(-400.0f, 50.0f);
    newGroundObstacle(820.0f, 200.0f);
    newGroundObstacle(-820.0f, 200.0f);
    newGroundObstacle(1275.0f, 150.0f);
    newGroundObstacle(-1275.0f, 150.0f);
    newGroundObstacle(-GameConfig::GROUND_WIDTH / 2.0f - (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    newGroundObstacle(GameConfig::GROUND_WIDTH / 2.0f + (GROUND_OBSTACLE_WIDTH / 2.0f), 600.0f, SKYBLUE);
    const int entityWidth = 10;
    const int entityHeight = 10;
    for (size_t i = 0; i < GameConfig::DUMMY_ENTITY_COUNT; i++) {
        entities.push_back(Entity(Random::getFloat(0, GameConfig::SOURCE_HEIGHT), Random::getFloat(0, GameConfig::SOURCE_WIDTH), 10, 10, YELLOW));
    }
}