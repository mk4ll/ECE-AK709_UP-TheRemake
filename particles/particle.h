#pragma once
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;          // seconds remaining

    Particle(const glm::vec3& pos, const glm::vec3& vel)
        : position(pos), velocity(vel), life(1.5f) {
    }
};
