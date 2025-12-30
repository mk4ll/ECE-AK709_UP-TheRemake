#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

class Rope {
public:
    // Create a rope between two points
    static Drawable* create(
        const glm::vec3& origin,
        const glm::vec3& end,
        int radialSegments = 12,
        float radius = 0.03f
    );
};
