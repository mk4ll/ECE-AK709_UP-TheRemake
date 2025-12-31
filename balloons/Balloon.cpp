#include "balloon.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

using namespace glm;

Balloon::Balloon(Drawable* mesh)
    : m_mesh(mesh),
    m_rope(nullptr),
    m_scale(0.5f),
    m_inflateSpeed(0.4f),
    m_maxScale(1.2f),
    m_attached(true),
    m_released(false),
    m_popped(false)
{
    // initial values for physics
    m_body.position = vec3(0.0f);
    m_body.velocity = vec3(0.0f);
    m_body.mass = 0.8f;
}

void Balloon::attachRope(RopeInstance* rope) {
    m_rope = rope;
}

void Balloon::setAnchor(const vec3& anchor) {
    m_anchor = anchor;

    float ropeLen = (m_rope) ? m_rope->getLength() : 5.0f;

    // initial pos
    m_body.position = m_anchor + vec3(0.0f, ropeLen, 0.0f);
    m_body.velocity = vec3(0.0f);
}

void Balloon::inflate(float dt) {
    if (m_popped || m_released) return;

    m_scale = min(m_scale + m_inflateSpeed * dt, m_maxScale);
}

void Balloon::release() {
    m_attached = false;
    m_released = true;
}

void Balloon::pop() {
    m_popped = true;
}

void Balloon::update(float dt) {
    if (m_popped) return;

    // gravity
    vec3 gravity(0.0f, -9.8f * m_body.mass, 0.0f);
    m_body.applyForce(gravity);

    // buoyancy
    float buoyancyStrength = 18.0f;
    vec3 buoyancy(0.0f, buoyancyStrength * m_scale, 0.0f);
    m_body.applyForce(buoyancy);

    // damping and drag
    float damping = 1.8f;
    vec3 drag = -damping * m_body.velocity;
    m_body.applyForce(drag);

    // attached to rope --> tension
    if (m_attached && m_rope) {
        float ropeLen = m_rope->getLength();

        vec3 dir = m_body.position - m_anchor;
        float dist = length(dir);

        if (dist > 0.0001f) {
            vec3 n = normalize(dir);

            // spring force
            float k = 25.0f;
            vec3 spring = -k * (dist - ropeLen) * n;

            m_body.applyForce(spring);
        }
    }

    // update position
    m_body.integrate(dt);

    // update rope (if attached)
    if (m_rope && m_attached) {
        m_rope->update(m_anchor, m_body.position);
    }
}

void Balloon::draw(GLuint modelMatrixLocation) const {
    if (m_popped) return;

    // rope
    if (m_rope && m_attached) {
        m_rope->draw(modelMatrixLocation);
    }

    // balloon
    mat4 M(1.0f);
    M = translate(M, m_body.position);
    M = scale(M, vec3(m_scale));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}
