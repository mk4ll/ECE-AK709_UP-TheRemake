#pragma once
#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec3 force{ 0.0f };

    float mass = 1.0f;

    void applyForce(const glm::vec3& f) {
        force += f;
    }

    void integrate(float dt) {
        glm::vec3 acceleration = force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;
        force = glm::vec3(0.0f);
    }
};

