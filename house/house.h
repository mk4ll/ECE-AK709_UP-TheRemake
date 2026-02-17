#pragma once

#include <GL/glew.h>
#include <common/model.h>
#include <glm/glm.hpp>
#include <physics/forces.h>
#include <physics/rigidBody.h>
#include <vector>

class Balloon;
class WindSystem;

class House {
public:
    House(Drawable* mesh, const glm::vec3& initialPosition);

    // Physics simulation
    void applyForces(const std::vector<Balloon*>& balloons,
        WindSystem* windSystem = nullptr);
    void update(float dt);

    // Rendering
    void draw(GLuint modelMatrixLocation) const;

    // Getters
    const glm::vec3& getPosition() const { return m_body.position; }
    const glm::vec3& getVelocity() const { return m_body.velocity; }
    const glm::vec3& getInitialPosition() const { return m_initialPosition; }
    bool isFlying() const { return m_isFlying; }
    int getAttachedBalloonCount() const { return m_attachedBalloonCount; }

    // Rotation getters for rendering
    const glm::vec3& getRotation() const { return m_rotation; }
    float getTiltAngle() const { return m_tiltAngle; }
    const glm::vec3& getTiltAxis() const { return m_tiltAxis; }

    // Configuration
    static constexpr int BALLOON_THRESHOLD = 8;
    static constexpr float MAX_HEIGHT = 25.0f;
    static constexpr float HOUSE_MASS = 22.0f;

    static constexpr float HOUSE_WIDTH = 5.0f;
    static constexpr float HOUSE_HEIGHT = 5.0f;
    static constexpr float HOUSE_DEPTH = 5.0f;

    // Terrain collision
    using TerrainHeightFunc = float (*)(float x, float z);
    void setTerrainHeightFunction(TerrainHeightFunc func) {
        m_getTerrainHeight = func;
    }

    // External force application (for navigation systems)
    void applyExternalForce(const glm::vec3& force) { m_body.applyForce(force); }

    // Set control input for direct tilt control
    void setControlInput(const glm::vec3& inputDir) { m_controlInput = inputDir; }

private:
    Drawable* m_mesh;
    RigidBody m_body;

    glm::vec3 m_controlInput; // Desired movement direction from user

    glm::vec3 m_initialPosition;
    bool m_isFlying;
    int m_attachedBalloonCount;

    // Physics parameters
    float m_liftPerBalloon;
    float m_dragCoefficient;

    // Takeoff mechanics
    float m_takeoffTimer;
    float m_takeoffDelay;
    bool m_isTakingOff;

    // Terrain collision
    TerrainHeightFunc m_getTerrainHeight;

    // Rotation and tilt physics
    glm::vec3 m_rotation;        // Current rotation (euler angles)
    glm::vec3 m_angularVelocity; // Angular velocity
    float m_tiltAngle;           // Current tilt angle for rendering
    glm::vec3 m_tiltAxis;        // Tilt axis direction

    // Tilt physics parameters
    static constexpr float TILT_RESPONSE = 0.30f; // Increased from 0.15f
    static constexpr float TILT_DAMPING = 3.0f;   // Angular damping
    static constexpr float MAX_TILT_ANGLE = 0.12f; // ~7 degrees

    // Helper methods
    void updateTiltPhysics(float dt);
    void updateRotation(float dt);

    // AABB (Axis Aligned Bounding Box) for collision
    glm::vec3 m_minLocalAABB;
    glm::vec3 m_maxLocalAABB;
    void calculateLocalAABB();

public:
    void getWorldAABB(glm::vec3& min, glm::vec3& max) const;
};