#pragma once

#include <glm/glm.hpp>
#include <common/model.h>
#include "ropeInstance.h"

class Balloon {
public:

    Balloon(Drawable* mesh);

    void attachRope(RopeInstance* rope);
    void setAnchor(const glm::vec3& anchor);

    void inflate(float dt);
    void release();
    void pop();
    void update(float dt);
    void draw(GLuint modelMatrixLocation) const;

private:
    Drawable* m_mesh;
    RopeInstance* m_rope;

    glm::vec3 m_position;
    glm::vec3 m_anchor;
    glm::vec3 m_velocity;

    float m_scale;
    float m_inflateSpeed = 0.4f;
    float m_maxScale = 1.2f;

    bool m_attached;
    bool m_released;
    bool m_popped;
};
