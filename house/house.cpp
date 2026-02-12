#include "house.h"
#include "balloons/balloon.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

House::House(Drawable* mesh, const vec3& initialPosition)
    : m_mesh(mesh),
    m_initialPosition(initialPosition),
    m_isFlying(false),
    m_attachedBalloonCount(0),
    m_liftPerBalloon(15.0f),      // Each balloon provides 15N of lift
    m_dragCoefficient(5.0f)        // Air resistance
{
    // Initialize rigid body
    m_body.position = initialPosition;
    m_body.velocity = vec3(0.0f);
    m_body.mass = HOUSE_MASS;
    m_body.force = vec3(0.0f);
}

void House::applyForces(const std::vector<Balloon*>& balloons) {
    // Reset force accumulator
    m_body.force = vec3(0.0f);

    // Count attached, non-popped balloons
    m_attachedBalloonCount = 0;
    for (const auto* balloon : balloons) {
        if (balloon->isRopeAttached() && !balloon->isPopped()) {
            m_attachedBalloonCount++;
        }
    }

    // 1. GRAVITY
    m_body.applyForce(Forces::gravity(m_body.mass));

    // 2. LIFT from balloons
    if (m_attachedBalloonCount >= BALLOON_THRESHOLD) {
        m_isFlying = true;

        // Total lift force = No. of balloons * lift/balloon
        float totalLift = m_attachedBalloonCount * m_liftPerBalloon;
        vec3 liftForce = vec3(0.0f, totalLift, 0.0f);
        m_body.applyForce(liftForce);

    }
    else {
        m_isFlying = false;
    }

    // 3. AIR DRAG
    if (length(m_body.velocity) > 0.01f) {
        m_body.applyForce(Forces::drag(m_body.velocity, m_dragCoefficient));
    }

    // 4. HEIGHT LIMIT
    float currentHeight = m_body.position.y - m_initialPosition.y;
    if (currentHeight > MAX_HEIGHT) {
        // Apply strong downward force when exceeding max height
        float excess = currentHeight - MAX_HEIGHT;
        vec3 limitForce = vec3(0.0f, -excess * 50.0f, 0.0f);
        m_body.applyForce(limitForce);
    }
}

void House::update(float dt) {
    // Integrate physics
    m_body.integrate(dt);

    // GROUND CONSTRAINT - House cannot go below initial position
    if (m_body.position.y < m_initialPosition.y) {
        m_body.position.y = m_initialPosition.y;
        m_body.velocity.y = 0.0f;  // Stop downward velocity
    }

    // cout for debugging
    /*
    static float debugTimer = 0.0f;
    debugTimer += dt;
    if (debugTimer > 1.0f) {  // Print every second
        std::cout << "House Status:" << std::endl;
        std::cout << "  Balloons attached: " << m_attachedBalloonCount << "/" << BALLOON_THRESHOLD << std::endl;
        std::cout << "  Position: (" << m_body.position.y << ")" << std::endl;
        std::cout << "  Height: " << (m_body.position.y - m_initialPosition.y) << std::endl;
        std::cout << "  Flying: " << (m_isFlying ? "YES" : "NO") << std::endl;
        debugTimer = 0.0f;
    }
    */
}

void House::draw(GLuint modelMatrixLocation) const {
    glm::mat4 M(1.0f);
    M = glm::translate(M, m_body.position);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}