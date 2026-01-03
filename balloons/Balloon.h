#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

#include "../physics/rigidBody.h"
#include "../physics/forces.h"
#include "ropeInstance.h"

enum class BalloonState {
    Spawn,
    Physics
};

class Balloon {
public:
    Balloon(Drawable* mesh);

    // setup
    void setAnchor(const glm::vec3& anchor);
    void attach(float ropeLength);

    // simulation
    void applyForces();
    void update(float dt);

    // baloon-rope relation
    bool isAttached() const;
    const glm::vec3& getAnchor() const;
    const glm::vec3& getPosition() const;
    float getRopeLength() const;

    const glm::vec3& getFreeRopeAnchor() const;
    glm::vec3 getRopeStart() const;

    // render
    void draw(GLuint modelMatrixLocation) const;

    // balloon methods
    void release();
    void pop();
    void inflate();

private:
    Drawable* m_mesh;

    // physics
    RigidBody m_body;

    // rope
    glm::vec3 m_anchor;
    float m_ropeLength;
    bool m_attached;

    glm::vec3 m_freeRopeAnchor;

    // balloon properties
    float m_radius;

    // kinematics spawn using states
    BalloonState m_state;
    float m_spawnTimer;
};


