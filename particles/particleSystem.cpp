#include "particleSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>

using namespace glm;

static vec3 randomDir() {
    vec3 v(
        (rand() % 200 - 100) / 100.0f,
        (rand() % 200 - 100) / 100.0f,
        (rand() % 200 - 100) / 100.0f
    );
    return normalize(v);
}

ParticleSystem::ParticleSystem(const vec3& origin) {
    const int COUNT = 40;
    const float SPEED = 6.0f;

    for (int i = 0; i < COUNT; ++i) {
        vec3 vel = randomDir() * SPEED;
        m_particles.emplace_back(origin, vel);
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : m_particles) {
        if (p.life <= 0.0f) continue;

        p.velocity += vec3(0, -9.8f, 0) * dt; // gravity
        p.position += p.velocity * dt;
        p.life -= dt;
    }
}

bool ParticleSystem::isAlive() const {
    for (const auto& p : m_particles)
        if (p.life > 0.0f) return true;
    return false;
}

void ParticleSystem::draw(GLuint modelMatrixLocation) const {
    for (const auto& p : m_particles) {
        if (p.life <= 0.0f) continue;

        mat4 M(1.0f);
        M = translate(M, p.position);
        M = scale(M, vec3(0.05f));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
    }
}
