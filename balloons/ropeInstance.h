#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

class RopeInstance {
public:
    RopeInstance(float length);

    float getLength() const { return m_length; }

    void update(const glm::vec3& anchor, const glm::vec3& balloonPos);
    void draw(GLuint modelMatrixLocation) const;

private:
    glm::vec3 m_anchor;
    glm::vec3 m_balloonPos;
    float m_length;

    Drawable* m_mesh;
};
