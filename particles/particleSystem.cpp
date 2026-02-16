#include "particleSystem.h"
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static vec3 randomDir() {
    float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265f;
    float phi = ((float)rand() / RAND_MAX) * 3.14159265f;
    return vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

ParticleSystem::ParticleSystem(const vec3& origin, vec3& color)
    : m_color(color), m_persistent(false) {
    const int COUNT = 100;
    const float SPEED = 6.0f;
    for (int i = 0; i < COUNT; ++i) {
        vec3 vel = randomDir() * SPEED;
        m_particles.emplace_back(origin, vel, color);
    }
}

ParticleSystem* ParticleSystem::createCrashExplosion(const vec3& origin,
    int count) {
    ParticleSystem* ps = new ParticleSystem();
    ps->m_persistent = true;
    ps->m_color = vec3(1.0f, 0.85f, 0.2f);
    const float SPEED = 10.0f;
    for (int i = 0; i < count; ++i) {
        vec3 vel = randomDir() * SPEED * (0.5f + (float)rand() / RAND_MAX);
        vel.y = glm::abs(vel.y) * 1.5f;
        vec3 color = vec3(0.9f + 0.1f * (float)rand() / RAND_MAX,
            0.7f + 0.2f * (float)rand() / RAND_MAX,
            0.05f + 0.15f * (float)rand() / RAND_MAX);
        ps->m_particles.emplace_back(origin, vel, color, true);
    }
    return ps;
}

void ParticleSystem::update(float dt) {
    for (auto& p : m_particles) {
        if (!p.persistent && p.life <= 0.0f)
            continue;
        p.velocity += vec3(0, -9.8f, 0) * dt;
        p.position += p.velocity * dt;
        if (!p.persistent) {
            p.life -= dt;
        }
    }
}

bool ParticleSystem::isAlive() const {
    for (const auto& p : m_particles) {
        if (p.persistent || p.life > 0.0f)
            return true;
    }
    return false;
}

void ParticleSystem::draw(GLuint modelMatrixLocation, Drawable* mesh) const {
    if (!mesh)
        return;
    mesh->bind();
    for (const auto& p : m_particles) {
        if (!p.persistent && p.life <= 0.0f)
            continue;
        float alpha = p.persistent ? 1.0f : (p.life / p.initialLife);
        mat4 M(1.0f);
        M = translate(M, p.position);
        float sz = p.persistent ? 0.08f : 0.02f;
        M = scale(M, vec3(sz));
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
        mesh->draw();
    }
}