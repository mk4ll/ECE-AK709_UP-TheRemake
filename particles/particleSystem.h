#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>
#include "particle.h"

using namespace glm;

class ParticleSystem {
public:
    ParticleSystem(const vec3& origin, vec3& color);

    void spawnExplosion(const vec3& pos, int count);
    void update(float dt);
    void draw(GLuint modelMatrixLocation, Drawable* mesh) const;

    bool isAlive() const;

private:
    std::vector<Particle> m_particles;
    vec3 m_color; // store the color
};
