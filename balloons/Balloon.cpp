#include "balloon.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

Balloon::Balloon(Drawable* mesh)
    : m_mesh(mesh),
    m_rope(nullptr),
    m_scale(0.5f),
    m_attached(true),
    m_released(false),
    m_popped(false),
    m_velocity(0.0f)
{
}

void Balloon::attachRope(RopeInstance* rope) {
    m_rope = rope;
}

void Balloon::setAnchor(const glm::vec3& anchor) {
    m_anchor = anchor;
    m_position = anchor + glm::vec3(0.0f, 5.0f, 0.0f);;
}

void Balloon::inflate(float dt) {
    if (m_popped || m_released) return;
    m_scale = glm::min(m_scale + m_inflateSpeed * dt, m_maxScale);
}

void Balloon::release() {
    m_attached = false;
    m_released = true;
    m_velocity = glm::vec3(0.0f, 1.5f, 0.0f);
}

void Balloon::pop() {
    m_popped = true;
}

void Balloon::update(float dt) {
    if (m_popped) return;

    if (m_attached) {
        m_position = m_anchor + glm::vec3(0.0f, 5.0f, 0.0f);
    }
    else if (m_released) {
        m_position += m_velocity * dt;
        m_anchor += m_velocity * dt;
    }

    if (m_rope) {
        m_rope->update(m_anchor, m_position);
    }
}

void Balloon::draw(GLuint modelMatrixLocation) const {
    if (m_popped) return;

    // balloon
    mat4 M = translate(mat4(1.0f), m_position);
    M = scale(M, vec3(m_scale));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
    m_mesh->bind();
    m_mesh->draw();

    // rope
    if (m_rope) {
        m_rope->draw(modelMatrixLocation);
    }

}
