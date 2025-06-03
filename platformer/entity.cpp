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
    //DrawRectangleLines(aabb.min.x + offsetX, aabb.min.y + offsetY, aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y, RED);
}
void newGroundObstacle(float x, float height, Color color) {
    platforms.push_back(Entity(x, GameConfig::SOURCE_HEIGHT - GameConfig::GROUND_HEIGHT - height / 2, 50, height, color));
}