#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>
#include <physics/rigidBody.h>
#include <physics/forces.h>
#include <vector>

class Balloon;

class House {
public:
    House(Drawable* mesh, const glm::vec3& initialPosition);

    // Physics simulation
    void applyForces(const std::vector<Balloon*>& balloons);
    void update(float dt);

    // Rendering
    void draw(GLuint modelMatrixLocation) const;

    // Getters
    const glm::vec3& getPosition() const { return m_body.position; }
    const glm::vec3& getInitialPosition() const { return m_initialPosition; }
    bool isFlying() const { return m_isFlying; }
    int getAttachedBalloonCount() const { return m_attachedBalloonCount; }

    // Configuration
    static constexpr int BALLOON_THRESHOLD = 8;  // Minimum balloons needed to fly
    static constexpr float MAX_HEIGHT = 100.0f;   // Maximum height above initial position
    static constexpr float HOUSE_MASS = 20.0f;  // Mass of the house         ***
    
    static constexpr float HOUSE_WIDTH = 8.0f;    // X dimension
    static constexpr float HOUSE_HEIGHT = 10.0f;  // Y dimension  
    static constexpr float HOUSE_DEPTH = 8.0f;    // Z dimension

    // Terrain collision
    using TerrainHeightFunc = float(*)(float x, float z);
    void setTerrainHeightFunction(TerrainHeightFunc func) { m_getTerrainHeight = func; }

private:
    Drawable* m_mesh;
    RigidBody m_body;

    glm::vec3 m_initialPosition;  // Starting position (on ground)
    bool m_isFlying;
    int m_attachedBalloonCount;

    // Physics parameters
    float m_liftPerBalloon;       // Lift force each balloon provides
    float m_dragCoefficient;      // Air resistance

    // Takeoff mechanics
    float m_takeoffTimer;         // Time since enough balloons attached
    float m_takeoffDelay;         // Delay before liftoff starts (2-3 seconds)
    bool m_isTakingOff;           // In takeoff phase

    // Terrain collision
    TerrainHeightFunc m_getTerrainHeight;
};