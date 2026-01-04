#include "ropeInstance.h"
#include "rope.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

RopeInstance::RopeInstance(float length)
    : m_length(length),
    m_sag(0.0f),
    m_hanging(false)
{
    // mesh (0,0,0) -> (0,1,0)
    m_mesh = Rope::create(
        vec3(0), vec3(0, 1, 0), 
        12, // radial segments
        1.0f,
        Rope::DEFAULT_RADIUS
    );
}

void RopeInstance::updateBezier(const vec3& anchor, const vec3& end, bool hanging, float dt) {
    m_anchor = anchor;
    m_end = end;
    m_hanging = hanging;

    if (m_hanging) {
        // animate sag frame-by-frame
        m_sag += 3.0f * dt;
        m_sag = glm::min(m_sag, m_length * 0.6f);
    }
    else {
        m_sag = 0.0f;
    }
}


void RopeInstance::update(const vec3& anchor, const vec3& balloonPos) {
    m_anchor = anchor;
    m_balloonPos = balloonPos;
}

void RopeInstance::draw(GLuint modelMatrixLocation) const
{
    const int SEGMENTS = 16;

    glm::vec3 p0 = m_anchor;
    glm::vec3 p2 = m_end;

    glm::vec3 p1 = (p0 + p2) * 0.5f;
    p1.y -= m_sag;

    auto bezier = [&](float t) {
        float u = 1.0f - t;
        return u * u * p0 + 2 * u * t * p1 + t * t * p2;
        };

    for (int i = 0; i < SEGMENTS; ++i) {
        float t0 = float(i) / SEGMENTS;
        float t1 = float(i + 1) / SEGMENTS;

        glm::vec3 a = bezier(t0);
        glm::vec3 b = bezier(t1);

        glm::vec3 dir = b - a;
        float len = glm::length(dir);
        if (len < 0.0001f) continue;

        glm::vec3 n = glm::normalize(dir);
        glm::vec3 axis = glm::cross(glm::vec3(0, 1, 0), n);
        float angle = acos(glm::dot(glm::vec3(0, 1, 0), n));

        glm::mat4 M(1.0f);
        M = glm::translate(M, a);

        if (glm::length(axis) > 0.0001f)
            M = glm::rotate(M, angle, glm::normalize(axis));

        M = glm::scale(M, glm::vec3(1.0f, len, 1.0f));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
        m_mesh->bind();
        m_mesh->draw();
    }
}
