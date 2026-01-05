#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>

#include "../physics/rigidBody.h"
#include "../physics/forces.h"
#include "ropeInstance.h"
#include "../physics/collisionShapes.h"
#include "verletRope.h"


enum class BalloonState {
    Spawn,
    Physics,
    Popped
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

    // balloon-rope relation
    bool isAttached() const;
    const glm::vec3& getAnchor() const;
    const glm::vec3& getPosition() const;
    float getRopeLength() const;

    glm::vec3 getColor() const { return glm::vec3(0.85f, 0.10f, 0.10f); }
    VerletRope* m_verletRope;  // Verlet rope for when balloon pops

    const glm::vec3& getFreeRopeAnchor() const;
    glm::vec3 getRopeStart() const;

    bool isPopped() const;

    // render
    void draw(GLuint modelMatrixLocation) const;

    // balloon methods
    void release();
    void pop();
    void inflate();

    // House collision bounds
    glm::vec3 m_houseMin;
    glm::vec3 m_houseMax;

    //get house bounds
    void setHouseBounds(const glm::vec3& min, const glm::vec3& max);
    // verlet rope for rendering
    VerletRope* getVerletRope() const;

private:
    Drawable* m_mesh;

    // physics
    RigidBody m_body;

    // rope
    glm::vec3 m_anchor;
    float m_ropeLength;
    bool m_attached;

    glm::vec3 m_freeRopeAnchor;

    // rope physics
    AABB m_houseAABB;
    bool m_hasHouseCollision;

    // balloon properties
    float m_radius;

    // kinematics spawn using states
    BalloonState m_state;
    float m_spawnTimer;

    // pop flag
    bool m_popped;
    
};


