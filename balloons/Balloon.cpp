#include "balloon.h"

#include <iostream>
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


void Balloon::attachRope(Drawable* rope) {
    m_rope = rope;
}

void Balloon::setAnchor(const glm::vec3& anchor) {
    m_anchor = anchor;
    m_position = anchor;
}


void Balloon::inflate(float dt) {
    std::cout << "INFLATE CALLED, dt = " << dt << std::endl;

    if (m_popped || m_released) return;

    m_scale += m_inflateSpeed * dt;
}


void Balloon::release() {
    m_attached = false;
    m_released = true;
    m_velocity = glm::vec3(0.0f, 1.5f, 0.0f);
}


void Balloon::pop() {
    m_popped = true;
    // later: spawn particles
}

void Balloon::update(float dt) {

    if (m_popped)
        return;

    if (m_attached) {
        // locked on chimney
        m_position = m_anchor;
    }
    else if (m_released) {
        // free balloon
        m_position += m_velocity * dt;
    }
}


void Balloon::draw(GLuint modelMatrixLocation) const {
    if (m_popped) return;

    mat4 M = translate(mat4(1.0f), m_position);
    M = scale(M, vec3(m_scale));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}
