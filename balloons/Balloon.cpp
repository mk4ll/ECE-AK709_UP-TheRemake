#include "balloon.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Balloon::Balloon(Drawable* mesh)
    : m_mesh(mesh),
    m_ropeLength(5.0f),
    m_attached(true),
    m_radius(0.5f)
{
    m_body.position = vec3(0.0f, 0.0f, 0.0f);
    m_body.velocity = vec3(0.0f);
    m_body.mass = 0.8f;
}

void Balloon::setAnchor(const vec3& anchor) {
    m_anchor = anchor;
    m_body.position = m_anchor + vec3(0.0f, m_ropeLength, 0.0f);
    m_body.velocity = vec3(0.0f);
}

void Balloon::attach(float ropeLength) {
    m_ropeLength = ropeLength;
    m_attached = true;
}

void Balloon::release() {
    m_attached = false;
}

void Balloon::applyForces() {

    // gravity
    m_body.applyForce(Forces::gravity(m_body.mass));

    // buoyancy
    m_body.applyForce(Forces::buoyancy(m_radius));

    // air drag
    m_body.applyForce(Forces::drag(m_body.velocity, 2.0f));

    // rope constraint (spring + damper basically)
    if (m_attached) {
        m_body.applyForce(
            Forces::ropeSpringDamper(
                m_body.position,
                m_body.velocity,
                m_anchor,
                m_ropeLength,
                25.0f,   // stiffness k
                8.0f     // damping c
            )
        );
    }

}

void Balloon::update(float dt) {
    m_body.integrate(dt);
}

const vec3& Balloon::getPosition() const {
    return m_body.position;
}

void Balloon::draw(GLuint modelMatrixLocation) const {

    glm::mat4 M(1.0f);
    M = glm::translate(M, m_body.position);
    M = glm::scale(M, glm::vec3(m_radius));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}

