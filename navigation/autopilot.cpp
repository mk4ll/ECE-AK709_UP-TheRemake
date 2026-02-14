#include "autopilot.h"
#include "balloons/balloon.h"
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

using namespace glm;

Autopilot::Autopilot()
    : m_enabled(true),
    m_thrustForce(10.0f),
    m_arrivalDistance(5.0f),
    m_rotationSpeed(1.0f) {
}

Autopilot::~Autopilot() {}

void Autopilot::update(House* house, const Beacon* beacon, const std::vector<Balloon*>& balloons, float dt) {
    if (!m_enabled || !house || !beacon)
        return;

    // Only engage autopilot if the house is flying
    if (!house->isFlying())
        return;

    vec3 housePos = house->getPosition();
    vec3 beaconPos = beacon->getPosition();

    // Calculate distance to target
    float dist = distance(vec3(housePos.x, 0, housePos.z), vec3(beaconPos.x, 0, beaconPos.z));

    if (dist < m_arrivalDistance) {
        // STRICT ALIGNMENT CHECK: Only land if we are practically CENTERED
        if (dist < 1.5f) {
            // Stop horizontal movement
            vec3 damping = -house->getVelocity();
            damping.y = 0;                             // Don't dampen vertical fall
            house->applyExternalForce(damping * 5.0f); // Strong braking

            // Check if we are too high
            if (house->getAttachedBalloonCount() > 0 &&
                housePos.y > beacon->getHeight() + 1.0f) {
                m_releaseTimer += dt;
                // Release ONLY if we are NOT falling (Velocity > -0.1f implies hovering
                // or rising)
                if (m_releaseTimer > 3.0f && house->getVelocity().y > -0.1f) {
                    for (auto* b : balloons) {
                        if (b->isRopeAttached()) {
                            b->release();
                            printf("Autopilot: CENTERED! Releasing balloon for precision "
                                "landing! Dist: %.2f\n",
                                dist);
                            break; // One at a time
                        }
                    }
                    m_releaseTimer = 0.0f;
                }
            }
            return;
        }

        // If not centered yet, keep steering inward!

        // Dampen velocity to stop overshoot
        vec3 damping = -house->getVelocity() * 2.0f;
        house->applyExternalForce(damping);

        // Apply micro-thrust to center
        vec3 steeringForce = calculateSteeringForce(housePos, beaconPos);
        house->applyExternalForce(steeringForce * 0.5f);
        return;
    }

    // Approach Phase (within 20 units)
    if (dist < 20.0f) {
        // START CENTERING FIRST: Do NOT release balloons yet!
        // User requested: "first center with beacon location, then land"
        // So we skip the release logic here and just focus on steering.

        // Dampen thrust as we get closer to prevent overshoot
        float factor = dist / 20.0f;
        if (factor < 0.2f)
            factor = 0.2f;

        vec3 steeringForce = calculateSteeringForce(housePos, beaconPos);
        steeringForce *= factor;
        house->applyExternalForce(steeringForce);
        return;
    }

    // Reset release timer if not in landing zone
    m_releaseTimer = 0.0f;
    // Calculate steering force
    vec3 steeringForce = calculateSteeringForce(housePos, beaconPos);

    // Apply the force to the house
    house->applyExternalForce(steeringForce);

}

glm::vec3 Autopilot::calculateSteeringForce(const glm::vec3& currentPos,
    const glm::vec3& targetPos) {
    // 1. Get direction vector on the horizontal plane
    vec3 direction = targetPos - currentPos;
    direction.y = 0; // FLATTEN to XZ plane as requested!

    // 2. Normalize to get direction
    if (length(direction) < 0.001f)
        return vec3(0.0f);
    direction = normalize(direction);

    // 3. Scale by thrust
    return direction * m_thrustForce;
}

void Autopilot::applyUserControl(House* house, const glm::vec3& controlForce) {
    if (!house)
        return;
    house->applyExternalForce(controlForce);
}
