#include "collisionDetection.h"

void Collision::resolveGround(
    glm::vec3& position,
    glm::vec3& velocity,
    float groundHeight
) {
    if (position.y < groundHeight) {
        position.y = groundHeight;
        if (velocity.y < 0)
            velocity.y = 0;
    }
}
