#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>

// Verlet particle for rope simulation
struct VerletPoint {
    glm::vec3 position;      // current position
    glm::vec3 oldPosition;   // previous position - for Verlet integration
    bool pinned;             // if true, point cannot move (anchor point when attached)

    VerletPoint(const glm::vec3& pos, bool isPinned = false)
        : position(pos), oldPosition(pos), pinned(isPinned) {
    }
};

class VerletRope {
public:
    VerletRope(const glm::vec3& start, const glm::vec3& end, int segments);
    // update physics
    void update(float dt, int iterations = 5);
    // pin the first point (chimney anchor)
    void pinStart(const glm::vec3& position);
    // collision with house AABB
    void handleHouseCollision(const glm::vec3& houseMin, const glm::vec3& houseMax);
    // draw
    void draw(GLuint modelMatrixLocation, Drawable* ropeMesh) const;
    // get points
    const std::vector<VerletPoint>& getPoints() const { return m_points; }

private:
    std::vector<VerletPoint> m_points;
    float m_segmentLength;  // rest length of each segment
    int m_numSegments;

    // Verlet integration
    void applyVerlet(float dt);
    // constraints - rope segments at correct length
    void applyConstraints();
    // collision detection helper
    void constrainToAABB(glm::vec3& pos, const glm::vec3& min, const glm::vec3& max);
};