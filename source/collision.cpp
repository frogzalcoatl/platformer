#include "sahars_platformer.h"
#include <cmath>

void collision(Entity* entity, std::vector<Entity*> collidingEntities) {
    if (entity->isStatic) return;
    entity->isGrounded = false;
    for (size_t i = 0; i < collidingEntities.size(); i++) {
        if (!areAABBsColliding(entity->aabb, collidingEntities[i]->aabb) || collidingEntities[i] == entity) continue;
        float overlapX_rightPush = entity->aabb.max.x - collidingEntities[i]->aabb.min.x;
        float overlapX_leftPush = collidingEntities[i]->aabb.max.x - entity->aabb.min.x;
        float overlapY_topPush = entity->aabb.max.y - collidingEntities[i]->aabb.min.y;
        float overlapY_bottomPush = collidingEntities[i]->aabb.max.y - entity->aabb.min.y;
        float minOverlapX = std::fmin(overlapX_rightPush, overlapX_leftPush);
        float minOverlapY = std::fmin(overlapY_bottomPush, overlapY_topPush);
        const float penetration_threshold = 0.01f;
        if (minOverlapX < minOverlapY && minOverlapX > penetration_threshold) {
            entity->velocity.x = 0.0f;
            float displacementX;
            if (overlapX_rightPush < overlapX_leftPush) {
                displacementX = -overlapX_rightPush;
            }
            else {
                displacementX = overlapX_leftPush;
            }
            entity->position.x += displacementX;
        } else if (minOverlapY <= minOverlapX && minOverlapY > penetration_threshold) {
            entity->velocity.y = 0.0f;
            float displacementY;
            if (overlapY_topPush < overlapY_bottomPush) {
                displacementY = -overlapY_topPush;
            } else {
                displacementY = overlapY_bottomPush;
                entity->isGrounded = true;
            }
            entity->position.y += displacementY;
        }
    }
    entity->updateAABB();
}
AABB getBox(Vector2 position, float width, float height) {
    return { { position.x - width / 2.0f, position.y - height / 2.0f }, { position.x + width / 2.0f, position.y + height / 2.0f } };
};
bool areAABBsColliding(AABB box1, AABB box2) {
    return (
        box1.max.x > box2.min.x &&
        box1.min.x < box2.max.x &&
        box1.max.y > box2.min.y &&
        box1.min.y < box2.max.y
        );
}
Rectangle AABBtoRectangle(AABB aabb, int multiplier) {
    return { aabb.min.x * multiplier, aabb.min.y * multiplier, aabb.max.x * multiplier - aabb.min.x * multiplier, aabb.max.y * multiplier - aabb.min.y * multiplier };
}
