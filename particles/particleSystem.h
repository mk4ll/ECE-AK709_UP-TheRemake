#pragma once
#include "particle.h"
#include <GL/glew.h>
#include <common/model.h>
#include <glm/glm.hpp>
#include <vector>


using namespace glm;

class ParticleSystem {
public:
    ParticleSystem(const vec3& origin, vec3& color);

    // Factory for crash explosion (persistent particles)
    static ParticleSystem* createCrashExplosion(const vec3& origin, int count = 200);

    void spawnExplosion(const vec3& pos, int count);
    void update(float dt);
    void draw(GLuint modelMatrixLocation, Drawable* mesh) const;

    bool isAlive() const;

private:
    ParticleSystem()
        : m_color(vec3(0)), m_persistent(false) {
    } // private default ctor
    std::vector<Particle> m_particles;
    vec3 m_color; // store the color
    bool m_persistent = false;
};
