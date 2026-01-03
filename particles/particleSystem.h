#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "particle.h"

class ParticleSystem {
public:
    ParticleSystem(const glm::vec3& origin);

    void update(float dt);
    void draw(GLuint modelMatrixLocation) const;

    bool isAlive() const;

private:
    std::vector<Particle> m_particles;
};
