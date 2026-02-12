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
    static constexpr float MAX_HEIGHT = 50.0f;   // Maximum height above initial position
    static constexpr float HOUSE_MASS = 100.0f;  // Mass of the house

private:
    Drawable* m_mesh;
    RigidBody m_body;

    glm::vec3 m_initialPosition;  // Starting position (on ground)
    bool m_isFlying;
    int m_attachedBalloonCount;

    // Physics parameters
    float m_liftPerBalloon;       // Lift force each balloon provides
    float m_dragCoefficient;      // Air resistance
};