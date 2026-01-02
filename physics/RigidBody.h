#pragma once
#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;

    float mass;

    RigidBody()
        : position(0), velocity(0), force(0), mass(1.0f) {
    }

    void applyForce(const glm::vec3& f) {
        force += f;
    }

    void integrate(float dt) {
        if (mass <= 0.0f) return;

        glm::vec3 acceleration = force / mass;
        velocity += acceleration * dt;
        position += velocity * dt;

        force = glm::vec3(0.0f);
    }
};
