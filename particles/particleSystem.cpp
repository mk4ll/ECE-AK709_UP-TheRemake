#include "particleSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <cmath>

using namespace glm;

// This creates uniform distribution on a sphere
static vec3 randomDir() {
    // Spherical coordinates for uniform distribution
    float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.14159265f;  // [0, 2ð]
    float phi = ((float)rand() / RAND_MAX) * 3.14159265f;           // [0, ð]

    return vec3(sin(phi) * cos(theta),
        sin(phi) * sin(theta),
        cos(phi)
    );
}

// particles at origin colored
ParticleSystem::ParticleSystem(const vec3& origin, vec3& color)
    : m_color(color)  // Store the color
{
    const int COUNT = 100;        // number of particles
    const float SPEED = 6.0f;    // v0 for particles

    // COUNT particles
    for (int i = 0; i < COUNT; ++i) {
        vec3 vel = randomDir() * SPEED;  // random direction * speed

        // Create particle with origin position, velocity, and color
        m_particles.emplace_back(origin, vel, color);
    }
}

// update particles
void ParticleSystem::update(float dt) {
    for (auto& p : m_particles) {
        // skip dead particles
        if (p.life <= 0.0f) continue;
        //gravity
        p.velocity += vec3(0, -9.8f, 0) * dt;
        // update position
        p.position += p.velocity * dt;
        // decrease life
        p.life -= dt;
    }
}

// check if any particles are still alive
bool ParticleSystem::isAlive() const {
    for (const auto& p : m_particles) {
        if (p.life > 0.0f) return true;
    }
    return false;
}

// draw alive particles
void ParticleSystem::draw(GLuint modelMatrixLocation, Drawable* mesh) const {
    if (!mesh) return;

    mesh->bind();

    for (const auto& p : m_particles) {
        // skip dead particles
        if (p.life <= 0.0f) continue;

        // alpha for fade out effect
        float alpha = p.life / p.initialLife;  // init: 1.0f
                                               // at death: 0.0f

        // particle model Matrix
        mat4 M(1.0f);
        M = translate(M, p.position);     // translate to particle pos
        M = scale(M, vec3(0.02f));        // scale down

        // upload and draw
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

        mesh->draw();
    }
}