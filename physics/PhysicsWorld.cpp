#include "physicsWorld.h"

void PhysicsWorld::addBody(RigidBody* body) {
    bodies.push_back(body);
}

void PhysicsWorld::step(float dt) {
    for (auto* body : bodies) {
        body->integrate(dt);
    }
}
