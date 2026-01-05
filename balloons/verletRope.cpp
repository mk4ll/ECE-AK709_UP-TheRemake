#include "verletRope.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

using namespace glm;

VerletRope::VerletRope(const vec3& start, const vec3& end, int segments)
    : m_numSegments(segments)
{
    // calc segment length
    float totalLength = length(end - start);
    m_segmentLength = totalLength / segments;

    // divide the rope into points
    for (int i = 0; i <= segments; ++i) {
        float t = float(i) / segments;
        vec3 pos = mix(start, end, t);

        // first point is pinned (chimney)
        bool isPinned = (i == 0);
        m_points.emplace_back(pos, isPinned);
    }
}

void VerletRope::pinStart(const vec3& position) {
    if (!m_points.empty()) {
        m_points[0].position = position;
        m_points[0].oldPosition = position;
        m_points[0].pinned = true;
    }
}

void VerletRope::update(float dt, int iterations) {
    // Verlet integration (motion from previous frame + gravity)
    applyVerlet(dt);

    // constraints - apply multiple times for stability
    for (int i = 0; i < iterations; ++i) {
        applyConstraints();
    }
}

void VerletRope::applyVerlet(float dt) {
    const vec3 gravity(0.0f, -9.8f, 0.0f);

    for (auto& point : m_points) {
        if (point.pinned) continue;

        // Verlet integration: x_new = x + (x - x_old) + a*dt^2
        vec3 velocity = point.position - point.oldPosition;
        point.oldPosition = point.position;

        // gravity
        point.position += velocity + gravity * dt * dt;

        // air damping (to prevent infinite swinging)
        point.position += velocity * 0.98f * dt;
    }
}

void VerletRope::applyConstraints() {
    // distance constraints - keep rope segments at correct length
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        VerletPoint& p1 = m_points[i];
        VerletPoint& p2 = m_points[i + 1];

        vec3 delta = p2.position - p1.position;
        float currentLength = length(delta);

        if (currentLength < 0.0001f) continue;

        // correction to restore segment length
        float difference = (currentLength - m_segmentLength) / currentLength;
        vec3 correction = delta * 0.5f * difference;

        // apply correction (pinned points don't move)
        if (!p1.pinned) p1.position += correction;
        if (!p2.pinned) p2.position -= correction;
    }
}

void VerletRope::handleHouseCollision(const vec3& houseMin, const vec3& houseMax) {
    // check each rope point against house AABB
    for (auto& point : m_points) {
        if (point.pinned) continue;

        constrainToAABB(point.position, houseMin, houseMax);
    }
}

void VerletRope::constrainToAABB(vec3& pos, const vec3& min, const vec3& max) {
    // check if point is inside AABB
    if (pos.x >= min.x && pos.x <= max.x &&
        pos.y >= min.y && pos.y <= max.y &&
        pos.z >= min.z && pos.z <= max.z) {

        // point inside - push it out to nearest face
        float distances[6] = {
            pos.x - min.x,  // dist to left face
            max.x - pos.x,  // dist to right face
            pos.y - min.y,  // dist bottom face
            max.y - pos.y,  // dist to top face
            pos.z - min.z,  // dist to front face
            max.z - pos.z   // dist to back face
        };

        // get nearest face
        int minIdx = 0;
        float minDist = distances[0];
        for (int i = 1; i < 6; ++i) {
            if (distances[i] < minDist) {
                minDist = distances[i];
                minIdx = i;
            }
        }

        // throw point out through nearest face
        const float pushOut = 0.02f; // small offset to prevent re-collision
        switch (minIdx) {
        case 0: pos.x = min.x - pushOut; break;
        case 1: pos.x = max.x + pushOut; break;
        case 2: pos.y = min.y - pushOut; break;
        case 3: pos.y = max.y + pushOut; break;
        case 4: pos.z = min.z - pushOut; break;
        case 5: pos.z = max.z + pushOut; break;
        }
    }
}

void VerletRope::draw(GLuint modelMatrixLocation, Drawable* ropeMesh) const {
    if (!ropeMesh) return;

    // draw rope as segments between consecutive points
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        const vec3& p1 = m_points[i].position;
        const vec3& p2 = m_points[i + 1].position;

        vec3 dir = p2 - p1;
        float len = length(dir);

        if (len < 0.0001f) continue;

        vec3 n = normalize(dir);

        // rotation matrix to align rope segment with direction
        vec3 axis = cross(vec3(0, 1, 0), n);
        float angle = acos(dot(vec3(0, 1, 0), n));

        mat4 M(1.0f);
        M = translate(M, p1);

        if (length(axis) > 0.0001f)
            M = rotate(M, angle, normalize(axis));

        M = scale(M, vec3(1.0f, len, 1.0f));

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
        ropeMesh->bind();
        ropeMesh->draw();
    }
}