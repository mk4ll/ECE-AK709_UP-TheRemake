#pragma once
#include <glm/glm.hpp>

namespace Collision {

    void resolveGround(
        glm::vec3& position,
        glm::vec3& velocity,
        float groundHeight
    );
}
