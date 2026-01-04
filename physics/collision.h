#pragma once
#include <glm/glm.hpp>
#include "collisionShapes.h"

void handleAABBSphereCollision(AABB& box, Sphere& sphere);
bool checkForAABBSphereCollision(glm::vec3& pos,
    float r,
    const AABB& box,
    glm::vec3& n);
