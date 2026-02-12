#include "balloon.h"
#include "balloonTypes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <physics/collision.h>
#include <physics/collisionShapes.h>


using namespace glm;

Balloon::Balloon(Drawable* mesh)
    : m_mesh(mesh),
    m_ropeLength(5.0f),
    m_attached(true),
    m_radius(0.5f),
    m_state(BalloonState::Spawn),
    m_spawnTimer(0.0f),
    m_popped(false),
    m_verletRope(nullptr),
    m_type(BalloonType::CLASSIC),
    m_glitterTime(0.0f)
{
    m_body.position = vec3(0.0f, 0.0f, 0.0f);
    m_body.velocity = vec3(0.0f);
    m_body.mass = 1.2f;
}

// Constructor with type
Balloon::Balloon(Drawable* mesh, BalloonType type)
    : m_mesh(mesh),
    m_ropeLength(5.0f),
    m_attached(true),
    m_radius(0.5f),
    m_state(BalloonState::Spawn),
    m_spawnTimer(0.0f),
    m_popped(false),
    m_verletRope(nullptr),
    m_type(type),
    m_glitterTime(0.0f)
{
    m_body.position = vec3(0.0f, 0.0f, 0.0f);
    m_body.velocity = vec3(0.0f);
    m_body.mass = 1.2f;
}

// new constructor with innerObject for TRANSPARENT balloons
Balloon::Balloon(Drawable* mesh, BalloonType type, Drawable* innerObject)
    : m_mesh(mesh),
    m_innerObject(innerObject),
    m_ropeLength(5.0f),
    m_attached(true),
    m_radius(0.5f),
    m_state(BalloonState::Spawn),
    m_spawnTimer(0.0f),
    m_popped(false),
    m_verletRope(nullptr),
    m_type(type),
    m_glitterTime(0.0f)
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

void Balloon::updateAnchor(const vec3& anchor) {
    m_anchor = anchor;
}

bool Balloon::isPopped() const {
    return m_popped;
}   

void Balloon::attach(float ropeLength) {
    m_ropeLength = ropeLength;
    m_attached = true;
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
                Forces::ropeSpringDamper(m_body.position, m_body.velocity, m_anchor, m_ropeLength, 25.0f, 8.0f));
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

    // if popped: update Verlet rope instead of balloon physics
    if (m_popped && m_verletRope) {
        m_verletRope->update(dt, 5); // 5 constraint iterations
        m_verletRope->handleHouseCollision(m_houseMin, m_houseMax);
        return;
    }

    // Physics phase
    m_body.integrate(dt);

    Sphere s;
    s.x = m_body.position;
    s.r = m_radius;
    
    m_body.position = s.x;

    // if released: rope follows the ballon
    if (!m_attached) {

        m_freeRopeAnchor =
            m_body.position - glm::vec3(0.0f, m_ropeLength, 0.0f);
    }
}

bool Balloon::isAttached() const {
    return m_state == BalloonState::Physics;;
}

glm::vec3 Balloon::getRopeStart() const {
    if (m_attached) {
        return m_anchor; // chimney
    }

    // free rope
    return m_body.position - glm::vec3(0.0f, m_ropeLength, 0.0f);
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
    if (m_popped) return;
    glm::mat4 M(1.0f);
    M = glm::translate(M, m_body.position);
    M = glm::scale(M, glm::vec3(m_radius));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}

void Balloon::drawContent(GLuint modelMatrixLocation) const {
    if (m_popped) return;

    // if transparent, add obj inside
    if (m_type == BalloonType::TRANSPARENT && m_innerObject != nullptr) {
        glm::mat4 innerM(1.0f);
        innerM = glm::translate(innerM, m_body.position + vec3(0.0f, 0.75f, 0.0f));
        innerM = glm::rotate(innerM, -3.14f/4.0f, vec3(1.0f, 0.0f, 0.0f));
        innerM = glm::scale(innerM, glm::vec3(5.0f));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &innerM[0][0]);
        m_innerObject->bind();
        m_innerObject->draw();
    }
}

void Balloon::release() {
    m_attached = false;
}

void Balloon::pop() {
    if (m_popped) return;
    m_popped = true;
    m_attached = true;

    // Verlet rope from anchor to current balloon position
    int segments = 20; // number of rope segments for simulation
    m_verletRope = new VerletRope(m_anchor, m_body.position, segments);
}

void Balloon::inflate() {
    printf("Balloon inflating... ");
    return;
}

// get house bounds
void Balloon::setHouseBounds(const glm::vec3& min, const glm::vec3& max) {
    m_houseMin = min;
    m_houseMax = max;
}

// get Verlet rope pointer
VerletRope* Balloon::getVerletRope() const {
    return m_verletRope;
}
