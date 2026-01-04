#pragma once

#include <glm/glm.hpp>
#include <common/model.h>

class RopeInstance {
public:
    RopeInstance(float length);

    float getLength() const { return m_length; }

    void update(const glm::vec3& anchor, const glm::vec3& balloonPos);
    void draw(GLuint modelMatrixLocation) const;

    void updateBezier(const glm::vec3& anchor, const glm::vec3& end, bool hanging, float dt);


private:
    glm::vec3 m_anchor;
    glm::vec3 m_end;
    glm::vec3 m_balloonPos;
    float m_length;

    Drawable* m_mesh;

    // Bezier control
    float m_sag;          // hanging coeff
    bool m_hanging;       // hanging flag (bascially balloon popped flag)
};
