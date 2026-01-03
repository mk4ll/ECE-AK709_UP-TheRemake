#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

#include "../physics/rigidBody.h"
#include "../physics/forces.h"
#include "ropeInstance.h"

class Balloon {
public:
    Balloon(Drawable* mesh);

    // setup
    void setAnchor(const glm::vec3& anchor);
    void attach(float ropeLength);
    void release();

    // simulation
    void applyForces();
    void update(float dt);

    // baloon-rope relation
    bool isAttached() const;
    const glm::vec3& getAnchor() const;
    const glm::vec3& getPosition() const;
    float getRopeLength() const;

    // render
    void draw(GLuint modelMatrixLocation) const;


private:
    Drawable* m_mesh;

    // physics
    RigidBody m_body;

    // rope
    glm::vec3 m_anchor;
    float m_ropeLength;
    bool m_attached;

    // balloon properties
    float m_radius;

    // kinematics spawn

    bool m_simulationEnabled;
};
