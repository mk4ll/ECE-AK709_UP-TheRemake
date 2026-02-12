#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>

#include "../physics/rigidBody.h"
#include "../physics/forces.h"
#include "ropeInstance.h"
#include "../physics/collisionShapes.h"
#include "verletRope.h"

#include "balloonTypes.h" 


enum class BalloonState {
    Spawn,
    Physics,
    Popped
};

class Balloon {
public:
    Balloon(Drawable* mesh);
    Balloon(Drawable* mesh, BalloonType type);
    // separate constructor for transparent balloon with obj inside
    Balloon(Drawable* mesh, BalloonType type, Drawable* innerObject);

    // setup
    void setAnchor(const vec3& anchor);
    void attach(float ropeLength);

    // simulation
    void applyForces();
    void update(float dt);

    // balloon-rope relation
    bool isAttached() const;
    bool isRopeAttached() const { return m_attached; }
    const vec3& getAnchor() const;
    const vec3& getPosition() const;
    float getRopeLength() const;

    // balloon types
    BalloonType getType() const { return m_type; }
    vec3 getColor() const { return getBalloonColor(m_type); }
    float getGlitterTime() const { return m_glitterTime; }

    // add more balloons (collision detection)
    float getRadius() const { return m_radius; }
    // collision detection fix: smaller imaginary sphere under the main one
    vec3 getLowerSphereCenter() const { return m_body.position - vec3(0.0f, m_radius * 0.6f, 0.0f); }
    float getLowerSphereRadius() const { return m_radius * 0.5f; }

    RigidBody& getRigidBody() { return m_body; }
    const RigidBody& getRigidBody() const { return m_body; }

    VerletRope* m_verletRope;  // Verlet rope for when balloon pops

    const vec3& getFreeRopeAnchor() const;
    vec3 getRopeStart() const;

    bool isPopped() const;

    // render
    void draw(GLuint modelMatrixLocation) const;
    void drawContent(GLuint modelMatrixLocation) const; // for banana

    // balloon methods
    void release();
    void pop();
    void inflate();

    // House collision bounds
    vec3 m_houseMin;
    vec3 m_houseMax;

    //get house bounds
    void setHouseBounds(const vec3& min, const vec3& max);
    // verlet rope for rendering
    VerletRope* getVerletRope() const;

private:
    Drawable* m_mesh;
    Drawable* m_innerObject;

    // physics
    RigidBody m_body;

    // rope
    vec3 m_anchor;
    float m_ropeLength;
    bool m_attached;

    vec3 m_freeRopeAnchor;

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

    // balloon types and effects
    BalloonType m_type;
    float m_glitterTime;
    
};


