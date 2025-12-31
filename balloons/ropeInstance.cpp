#include "ropeInstance.h"
#include "rope.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

RopeInstance::RopeInstance(float length)
    : m_length(length)
{
    m_mesh = Rope::create(vec3(0), vec3(0, 1, 0));
}

void RopeInstance::update(const vec3& origin, const vec3& end) {
    m_origin = origin;
    m_end = end;
}

void RopeInstance::draw(GLuint modelMatrixLocation) const {
    vec3 dir = m_end - m_origin;
    float len = length(dir);
    vec3 n = normalize(dir);

    // rotation from Y axis to dir
    vec3 axis = cross(vec3(0, 1, 0), n);
    float angle = acos(dot(vec3(0, 1, 0), n));

    mat4 M(1.0f);
    M = translate(M, m_origin);
    if (length(axis) > 0.0001f)
        M = rotate(M, angle, normalize(axis));
    M = scale(M, vec3(1.0f, len, 1.0f));

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    m_mesh->bind();
    m_mesh->draw();
}
