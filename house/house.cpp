#include "house.h"
#include "balloons/balloon.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

using namespace glm;

House::House(Drawable* mesh, const vec3& initialPosition)
    : m_mesh(mesh), m_initialPosition(initialPosition), m_isFlying(false),
    m_attachedBalloonCount(0), m_liftPerBalloon(15.0f),
    m_dragCoefficient(5.0f), m_takeoffTimer(0.0f), m_takeoffDelay(10.0f),
    m_isTakingOff(false), m_getTerrainHeight(nullptr), m_rotation(0.0f),
    m_angularVelocity(0.0f), m_tiltAngle(0.0f), m_tiltAxis(1.0f, 0.0f, 0.0f) {
    m_body.position = initialPosition;
    m_body.velocity = vec3(0.0f);
    m_body.mass = HOUSE_MASS;
    m_body.force = vec3(0.0f);
}

void House::applyForces(const std::vector<Balloon*>& balloons,
    WindSystem* windSystem) {
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
        m_isTakingOff = true;
    }
    else {
        m_isTakingOff = false;
        m_takeoffTimer = 0.0f;
        m_isFlying = false;
    }

    if (m_isTakingOff) {
        bool onGround = (m_body.position.y <= m_initialPosition.y + 0.1f);

        if (onGround && m_takeoffTimer < m_takeoffDelay) {
            // Building tension phase
            float tensionFactor = m_takeoffTimer / m_takeoffDelay;
            float weakLift =
                m_attachedBalloonCount * m_liftPerBalloon * 0.3f * tensionFactor;
            m_body.applyForce(vec3(0.0f, weakLift, 0.0f));
            float groundFriction = -m_body.velocity.y * 100.0f;
            m_body.applyForce(vec3(0.0f, groundFriction, 0.0f));

            // STRICTLY clamp horizontal position to initial position to prevent drift
            m_body.position.x = m_initialPosition.x;
            m_body.position.z = m_initialPosition.z;
            m_body.velocity.x = 0.0f;
            m_body.velocity.z = 0.0f;
        }
        else {
            // Taking off!
            m_isFlying = true;

            float liftupTime = m_takeoffTimer - m_takeoffDelay;
            // Start from 30% lift (matching tension phase) to 100% lift over 5
            // seconds
            float rampProgress = glm::clamp(liftupTime / 5.0f, 0.0f, 1.0f);
            float liftupFactor = glm::mix(0.3f, 1.0f, rampProgress);

            float totalLift =
                m_attachedBalloonCount * m_liftPerBalloon * liftupFactor;
            vec3 liftForce = vec3(0.0f, totalLift, 0.0f);
            m_body.applyForce(liftForce);
        }
    }

    // 3. AIR DRAG
    if (length(m_body.velocity) > 0.01f) {
        m_body.applyForce(Forces::drag(m_body.velocity, m_dragCoefficient));
    }

    // 4. WIND FORCES (NEW!)
    if (m_isFlying) {
        vec3 windForce = vec3(0.0f); // i need to add value
        m_body.applyForce(windForce);
    }

    // 5. HEIGHT LIMIT (Soft Ceiling)
    float currentHeight = m_body.position.y - m_initialPosition.y;
    if (currentHeight > MAX_HEIGHT) {
        float excess = currentHeight - MAX_HEIGHT;
        // Spring force (push down)
        vec3 limitForce = vec3(0.0f, -excess * 20.0f, 0.0f);
        // Damping force (resist upward velocity) to prevent bouncing
        if (m_body.velocity.y > 0.0f) {
            limitForce.y -= m_body.velocity.y * 5.0f;
        }
        m_body.applyForce(limitForce);
    }
}

void House::update(float dt) {
    if (m_isTakingOff) {
        m_takeoffTimer += dt;
    }

    // Update rotation and tilt based on forces (must be done BEFORE integration
    // clears forces)
    updateTiltPhysics(dt);

    // Integrate physics
    m_body.integrate(dt);

    updateRotation(dt);

    // TERRAIN COLLISION
    if (m_getTerrainHeight != nullptr) {
        const float halfWidth = HOUSE_WIDTH * 0.5f;
        const float halfDepth = HOUSE_DEPTH * 0.5f;

        vec3 corners[5] = { m_body.position + vec3(-halfWidth, 0, -halfDepth),
                           m_body.position + vec3(halfWidth, 0, -halfDepth),
                           m_body.position + vec3(-halfWidth, 0, halfDepth),
                           m_body.position + vec3(halfWidth, 0, halfDepth),
                           m_body.position };

        float maxTerrainHeight = -1e6f;
        for (int i = 0; i < 5; ++i) {
            float terrainHeight = m_getTerrainHeight(corners[i].x, corners[i].z);
            maxTerrainHeight = glm::max(maxTerrainHeight, terrainHeight);
        }

        float houseBottom = m_body.position.y;

        if (houseBottom < maxTerrainHeight) {
            float penetration = maxTerrainHeight - houseBottom;
            m_body.position.y += penetration;

            if (m_body.velocity.y < 0.0f) {
                float restitution = 0.1f;
                m_body.velocity.y = -m_body.velocity.y * restitution;

                if (glm::abs(m_body.velocity.y) < 0.5f) {
                    m_body.velocity.y = 0.0f;
                }
            }

            // Reset tilt on ground contact
            m_angularVelocity *= 0.8f;

            float groundFrictionFactor = 0.95f;
            m_body.velocity.x *= groundFrictionFactor;
            m_body.velocity.z *= groundFrictionFactor;
        }
    }
    else {
        if (m_body.position.y < m_initialPosition.y) {
            m_body.position.y = m_initialPosition.y;
            m_body.velocity.y = 0.0f;
            m_angularVelocity *= 0.8f;
        }
    }
}

void House::updateTiltPhysics(float dt) {
    if (!m_isFlying || m_body.position.y <= m_initialPosition.y + 0.5f) {
        // On ground - gradually return to upright
        m_angularVelocity *= 0.9f;
        m_tiltAngle *= 0.9f;
        return;
    }
    // Calculate desired tilt
    // STRATEGY: Combine Velocity and Force for responsiveness.
    // 1. Velocity gives the "steady state" tilt (banking in a turn).
    // 2. Force gives the "immediate" tilt (initial lurch when accelerating).

    vec3 horizontalVel = vec3(m_body.velocity.x, 0.0f, m_body.velocity.z);
    vec3 horizontalForce = vec3(m_body.force.x, 0.0f, m_body.force.z);

    vec3 tiltTargetDir = vec3(0.0f);
    float tiltMagnitude = 0.0f;

    if (length(horizontalForce) > 1.0f) {
        // High force applied? Tilt in that direction!
        tiltTargetDir = normalize(horizontalForce);
        tiltMagnitude =
            length(horizontalForce) * 0.05f; // Scaling factor for force-based tilt
    }
    else if (length(horizontalVel) > 0.1f) {
        // Moving fast? Tilt in direction of movement
        tiltTargetDir = normalize(horizontalVel);
        tiltMagnitude = length(horizontalVel) * TILT_RESPONSE;
    }

    // Clamp magnitude
    if (tiltMagnitude > 0.001f) {
        // Tilt axis perpendicular to target direction
        // FIX: Swapped cross product order to fix inverted tilt (inverted right
        // hand rule)
        vec3 desiredTiltAxis = normalize(cross(vec3(0, 1, 0), tiltTargetDir));

        // Smoothly blend axis (avoid instant snaps)
        // Only update axis if we have a valid target
        m_tiltAxis = normalize(glm::mix(m_tiltAxis, desiredTiltAxis, 5.0f * dt));

        // Smoothly interpolate to desired tilt
        // Use m_body.force to see if we are actively steering -> faster response
        float responseSpeed = (length(horizontalForce) > 1.0f) ? 5.0f : 2.0f;

        float angleDiff = tiltMagnitude - m_tiltAngle;
        m_angularVelocity += m_tiltAxis * angleDiff * responseSpeed * dt;

    }
    else {
        // No movement/force - return to upright
        m_angularVelocity -= m_angularVelocity * 2.0f * dt;
    }

    // Apply angular damping
    m_angularVelocity *= (1.0f - TILT_DAMPING * dt);

    // Update tilt angle from angular velocity
    float angularSpeed = length(m_angularVelocity);
    if (angularSpeed > 0.001f) {
        m_tiltAngle += angularSpeed * dt;
        m_tiltAngle = glm::clamp(m_tiltAngle, -MAX_TILT_ANGLE, MAX_TILT_ANGLE);
    }
    else {
        // Gradually return to zero tilt
        m_tiltAngle *= 0.75f;
    }
}

void House::updateRotation(float dt) {
    m_rotation.y += m_angularVelocity.y * dt * 0.1f; // Very subtle rotation
}

void House::draw(GLuint modelMatrixLocation) const {
    glm::mat4 M(1.0f);

    // Translate to position
    M = glm::translate(M, m_body.position);

    // Apply tilt rotation
    if (abs(m_tiltAngle) > 0.001f) {
        M = glm::rotate(M, m_tiltAngle, m_tiltAxis);
    }

    // Apply any Y-axis rotation (very subtle)
    if (abs(m_rotation.y) > 0.001f) {
        M = glm::rotate(M, m_rotation.y, vec3(0, 1, 0));
    }

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}