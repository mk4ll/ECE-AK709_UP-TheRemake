#pragma once
#include <vector>
#include "rigidBody.h"

class PhysicsWorld {
public:
    void addBody(RigidBody* body);
    void step(float dt);

private:
    std::vector<RigidBody*> bodies;
};
