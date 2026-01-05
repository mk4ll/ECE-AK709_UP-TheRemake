#pragma once
#include <glm/glm.hpp>

using namespace glm;

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float life;          // seconds remaining
    float initialLife;   // for alpha blending


    Particle(const vec3& pos, const vec3& vel, const vec3& col)
        : position(pos), velocity(vel), color(col), life(0.8f), initialLife(0.8f) {
    }
};
