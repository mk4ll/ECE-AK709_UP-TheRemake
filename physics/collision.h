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

// new collision functions
bool checkDualSphereCollision(const Sphere& mainSphere1, const Sphere& lowerSphere1,
                              const Sphere& mainSphere2, const Sphere& lowerSphere2);

void handleDualSphereCollision(Sphere& mainSphere1, Sphere& lowerSphere1,
                               Sphere& mainSphere2, Sphere& lowerSphere2,
                               vec3& vel1, vec3& vel2,
                               float mass1, float mass2);
