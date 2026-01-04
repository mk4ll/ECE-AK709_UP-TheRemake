#pragma once
#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct Sphere {
    glm::vec3 x;   // position (same naming as lab 7)
    float r;       // radius
};
