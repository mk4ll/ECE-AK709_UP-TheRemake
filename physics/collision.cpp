#include "collision.h"
using namespace glm;

void handleAABBSphereCollision(AABB& box, Sphere& sphere) {
    vec3 n;
    if (checkForAABBSphereCollision(sphere.x, sphere.r, box, n)) {
        // απλό response: σταματάμε την κίνηση προς το normal
        // (όχι bounce για balloon)
    }
}

bool checkForAABBSphereCollision(glm::vec3& pos,
    float r,
    const AABB& box,
    glm::vec3& n)
{
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
