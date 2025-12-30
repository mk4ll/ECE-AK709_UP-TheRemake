#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

class Rope {
public:
    static constexpr float DEFAULT_LENGTH = 5.0f;
    static constexpr float DEFAULT_RADIUS = 0.01f;

    // create a rope between two points
    static Drawable* create(
        const glm::vec3& origin,
        const glm::vec3& direction = glm::vec3(0, 1, 0),
        int radialSegments = 12,
        float length = DEFAULT_LENGTH,
        float radius = DEFAULT_RADIUS
    );
};
