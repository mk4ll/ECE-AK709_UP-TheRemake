#pragma once
#include <glm/glm.hpp>

namespace Forces {

    inline glm::vec3 gravity(float mass) {
        return glm::vec3(0, -9.8f * mass, 0);
    }

    inline glm::vec3 buoyancy(float radius) {
        const float rho = 1.225f;
        const float g = 9.8f;
        const float pi = 3.1415926f;

        float V = (4.0f / 3.0f) * pi * radius * radius * radius;

        float liftMultiplier = 6.0f;
        return glm::vec3(0, rho * V * g * liftMultiplier, 0);
    }

    inline glm::vec3 drag(const glm::vec3& v, float kd) {
        return -kd * v;
    }

    inline glm::vec3 ropeSpringDamper(
        const glm::vec3& pos,
        const glm::vec3& vel,
        const glm::vec3& anchor,
        float restLength,
        float stiffness,
        float damping
    ) {
        glm::vec3 d = pos - anchor;
        float dist = glm::length(d);
        if (dist < restLength) return glm::vec3(0.0f);

        glm::vec3 n = d / dist;

        float x = dist - restLength;
        glm::vec3 Fs = -stiffness * x * n;

        float v_rel = glm::dot(vel, n);
        glm::vec3 Fd = -damping * v_rel * n;

        return Fs + Fd;
    }

}
