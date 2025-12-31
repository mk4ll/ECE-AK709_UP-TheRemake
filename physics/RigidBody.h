#pragma once
#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 pos{ 0.0f };
    glm::vec3 vel{ 0.0f };
    glm::vec3 force{ 0.0f };

    float mass = 1.0f;

    void integrate(float dt) {
        glm::vec3 acc = force / mass;
        vel += acc * dt;
        pos += vel * dt;
        force = glm::vec3(0.0f);
    }
};
