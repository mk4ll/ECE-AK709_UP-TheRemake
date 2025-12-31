#pragma once
#include <glm/glm.hpp>
#include <common/model.h>

class RopeInstance {
public:
    RopeInstance(float length);

    float getLength() const { return m_length; }

    void update(const glm::vec3& origin, const glm::vec3& end);
    void draw(GLuint modelMatrixLocation) const;

private:
    glm::vec3 m_origin;
    glm::vec3 m_end;
    float m_length;
    Drawable* m_mesh;
};
