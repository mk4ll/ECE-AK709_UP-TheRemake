#pragma once
#include <glm/glm.hpp>
#include "collisionShapes.h"

using namespace glm;

void handleAABBSphereCollision(AABB& box, Sphere& sphere);
bool checkForAABBSphereCollision(glm::vec3& pos, float r, const AABB& box, glm::vec3& n);

bool checkSphereSphereCollision(const Sphere& s1, const Sphere& s2);
void handleSphereSphereCollision(Sphere& s1, Sphere& s2,
                                 vec3& vel1, vec3& vel2,
                                 float mass1, float mass2);
