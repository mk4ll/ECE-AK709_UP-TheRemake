#include "collision.h"
#include <glm/glm.hpp>

#include <vector>
#include <limits>

using namespace glm;
using namespace std;

void handleAABBSphereCollision(AABB& box, Sphere& sphere) {
    vec3 n;
    if (checkForAABBSphereCollision(sphere.x, sphere.r, box, n)) {
    }
}

bool checkForAABBSphereCollision(vec3& pos, float r, const AABB& box, vec3& n) {
    if (pos.x - r < box.min.x) {
        float d = box.min.x - (pos.x - r);
        pos.x += d;
        n = vec3(-1, 0, 0);
    }
    else if (pos.x + r > box.max.x) {
        float d = box.max.x - (pos.x + r);
        pos.x += d;
        n = vec3(1, 0, 0);
    }
    else if (pos.y - r < box.min.y) {
        float d = box.min.y - (pos.y - r);
        pos.y += d;
        n = vec3(0, -1, 0);
    }
    else if (pos.y + r > box.max.y) {
        float d = box.max.y - (pos.y + r);
        pos.y += d;
        n = vec3(0, 1, 0);
    }
    else if (pos.z - r < box.min.z) {
        float d = box.min.z - (pos.z - r);
        pos.z += d;
        n = vec3(0, 0, -1);
    }
    else if (pos.z + r > box.max.z) {
        float d = box.max.z - (pos.z + r);
        pos.z += d;
        n = vec3(0, 0, 1);
    }
    else {
        return false;
    }

    return true;
}


// SPHERE-SPHERE COLLISION


// check function
bool checkSphereSphereCollision(const Sphere& s1, const Sphere& s2) {
    vec3 diff = s1.x - s2.x;
    float distSquared = dot(diff, diff);
    float radiusSum = s1.r + s2.r;

    return distSquared < (radiusSum * radiusSum);
}

// collision handling
void handleSphereSphereCollision(Sphere& s1, Sphere& s2,
                                 vec3& vel1, vec3& vel2,
                                 float mass1, float mass2) {
    // collsiion axis nomal
    vec3 collisionNormal = s1.x - s2.x;
    float distance = length(collisionNormal);

    // safety
    if (distance < 0.0001f) {
        collisionNormal = vec3(0.0f, 1.0f, 0.0f);
        distance = 0.0001f;
    }

    collisionNormal = normalize(collisionNormal);

    // separation
    float overlap = (s1.r + s2.r) - distance;
    float totalMass = mass1 + mass2;

    // collision equations
    s1.x += collisionNormal * (overlap * (mass2 / totalMass));
    s2.x -= collisionNormal * (overlap * (mass1 / totalMass));

    // elastic collision
    vec3 relativeVelocity = vel1 - vel2;
    float velocityAlongNormal = dot(relativeVelocity, collisionNormal);

    if (velocityAlongNormal > 0) return;

    // Coefficient of restitution (0 = non-elastic, 1 = fully elastic)
    float restitution = 0.8f; // a bit bouncy for realistic behaviour

    // impulse
    float j = -(1.0f + restitution) * velocityAlongNormal;
    j /= (1.0f / mass1) + (1.0f / mass2);

    // apply impulse
    vec3 impulse = j * collisionNormal;
    vel1 += impulse / mass1;
    vel2 -= impulse / mass2;
}

bool checkDualSphereCollision(const Sphere& mainSphere1, const Sphere& lowerSphere1,
    const Sphere& mainSphere2, const Sphere& lowerSphere2) {
    // collision check for all combinations
    return checkSphereSphereCollision(mainSphere1, mainSphere2) ||
        checkSphereSphereCollision(mainSphere1, lowerSphere2) ||
        checkSphereSphereCollision(lowerSphere1, mainSphere2) ||
        checkSphereSphereCollision(lowerSphere1, lowerSphere2);
}

void handleDualSphereCollision(Sphere& mainSphere1, Sphere& lowerSphere1,
    Sphere& mainSphere2, Sphere& lowerSphere2,
    vec3& vel1, vec3& vel2,
    float mass1, float mass2) {

    // get closest pair of spheres
    float minDist = std::numeric_limits<float>::max();
    Sphere* closestA = nullptr;
    Sphere* closestB = nullptr;

    struct SpherePair {
        Sphere* a;
        Sphere* b;
        float dist;
    };

    std::vector<SpherePair> pairs = {
        {&mainSphere1, &mainSphere2, length(mainSphere1.x - mainSphere2.x) - (mainSphere1.r + mainSphere2.r)},
        {&mainSphere1, &lowerSphere2, length(mainSphere1.x - lowerSphere2.x) - (mainSphere1.r + lowerSphere2.r)},
        {&lowerSphere1, &mainSphere2, length(lowerSphere1.x - mainSphere2.x) - (lowerSphere1.r + mainSphere2.r)},
        {&lowerSphere1, &lowerSphere2, length(lowerSphere1.x - lowerSphere2.x) - (lowerSphere1.r + lowerSphere2.r)}
    };

    // get closest pair
    for (const auto& pair : pairs) {
        if (pair.dist < minDist) {
            minDist = pair.dist;
            closestA = pair.a;
            closestB = pair.b;
        }
    }

    if (closestA && closestB && minDist < 0.0f) {
        handleSphereSphereCollision(*closestA, *closestB, vel1, vel2, mass1, mass2);
    }
}