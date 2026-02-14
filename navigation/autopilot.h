#pragma once

#include <balloons/balloon.h>
#include <beacon/beacon.h>
#include <glm/glm.hpp>
#include <house/house.h>
#include <vector>

class Balloon;

class Autopilot {
public:
    Autopilot();
    ~Autopilot();

    // Main update function to guide the house
    void update(House* house, const Beacon* beacon, const std::vector<Balloon*>& balloons, float dt);

    // Navigation control
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // User overrides
    void applyUserControl(House* house, const glm::vec3& controlForce);

private:
    bool m_enabled;
    float m_thrustForce;
    float m_arrivalDistance;
    float m_rotationSpeed;
    float m_releaseTimer;

    // Helper to calculate steering force
    glm::vec3 calculateSteeringForce(const glm::vec3& currentPos, const glm::vec3& targetPos);
};
