#include "balloon.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Balloon::Balloon(Drawable* mesh)
    : m_mesh(mesh),
    m_ropeLength(5.0f),
    m_attached(true),
    m_radius(0.5f),
    m_state(BalloonState::Spawn),
    m_spawnTimer(0.0f)
{
    m_body.position = vec3(0.0f, 0.0f, 0.0f);
    m_body.velocity = vec3(0.0f);
    m_body.mass = 1.2f;
}

void Balloon::setAnchor(const vec3& anchor) {
    m_anchor = anchor;
    m_body.position = m_anchor + glm::vec3(0.0f, 1.2f, 0.0f);

    m_body.velocity = glm::vec3(0.0f);

    m_state = BalloonState::Spawn;
    m_spawnTimer = 0.0f;
}

void Balloon::attach(float ropeLength) {
    m_ropeLength = ropeLength;
    m_attached = true;
}

void Balloon::release() {
    m_attached = false;

    m_freeRopeAnchor = m_body.position - glm::vec3(0.0f, m_ropeLength, 0.0f);
}

void Balloon::applyForces() {
    if (m_state != BalloonState::Physics)
        return;
    // gravity
    m_body.applyForce(Forces::gravity(m_body.mass));

    // buoyancy
    m_body.applyForce(Forces::buoyancy(m_radius));

    // air drag
    m_body.applyForce(Forces::drag(m_body.velocity, 2.0f));

    // rope constraint (spring + damper basically)
    if (m_attached) {
        glm::vec3 d = m_body.position - m_anchor;
        float dist = glm::length(d);

        if (dist > m_ropeLength) {
            m_body.applyForce(
                Forces::ropeSpringDamper(
                    m_body.position,
                    m_body.velocity,
                    m_anchor,
                    m_ropeLength,
                    25.0f,
                    8.0f
                )
            );
        }
    }
}

void Balloon::update(float dt) {
    if (m_state == BalloonState::Spawn) {

        m_spawnTimer += dt;

        // short pause to make the spawn visible
        if (m_spawnTimer > 0.15f) {
            m_state = BalloonState::Physics;
        }
        return;
    }

    if (!m_attached) {
        // rope follows ballon
        m_freeRopeAnchor =
            m_body.position - glm::vec3(0.0f, m_ropeLength, 0.0f);
    }

    // Physics phase
    m_body.integrate(dt);
}

bool Balloon::isAttached() const {
    return m_state == BalloonState::Physics;;
}

const glm::vec3& Balloon::getAnchor() const {
    return m_anchor;
}

const glm::vec3& Balloon::getPosition() const {
    return m_body.position;
}

float Balloon::getRopeLength() const {
    return m_ropeLength;
}

const glm::vec3& Balloon::getFreeRopeAnchor() const {
    return m_freeRopeAnchor;
}

void Balloon::draw(GLuint modelMatrixLocation) const {

    glm::mat4 M(1.0f);
    M = glm::translate(M, m_body.position);
    M = glm::scale(M, glm::vec3(m_radius));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}