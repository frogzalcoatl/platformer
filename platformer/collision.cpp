#include "sahars_platformer.h"
#include <cmath>

void collision(Entity& entity) {
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
            entity.velocity.x = 0.0f;
            float displacementX;
            if (overlapX_rightPush < overlapX_leftPush) {
                displacementX = -overlapX_rightPush;
            }
            else {
                displacementX = overlapX_leftPush;
            }
            entity.position.x += displacementX;
        }
        else if (minOverlapY <= minOverlapX && minOverlapY > penetration_threshold) {
            entity.velocity.y = 0.0f;
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
Box getBox(Vector2 position, float width, float height) {
    return { { position.x - width / 2.0f, position.y - height / 2.0f }, { position.x + width / 2.0f, position.y + height / 2.0f } };
};