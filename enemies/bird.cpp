#include "bird.h"
#include <cmath>
#include <common/model.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Bird::Bird(Drawable** frames, int frameCount, vec3 center, float orbitRadius,
    float speed, float height, float startAngle)
    : m_frames(frames), m_frameCount(frameCount), m_animTime(0.0f),
    m_animSpeed(12.0f), // 12 fps wing flap
    m_center(center), m_orbitRadius(orbitRadius), m_speed(speed),
    m_angle(startAngle), m_height(height), m_collisionRadius(2.0f),
    m_scale(1.5f) {
    // Initialize position
    m_position = vec3(m_center.x + m_orbitRadius * cos(m_angle), m_height,
        m_center.z + m_orbitRadius * sin(m_angle));
}

void Bird::update(float dt) {
    // Advance orbit angle
    m_angle += m_speed * dt;
    if (m_angle > 6.28318f)
        m_angle -= 6.28318f;

    // Calculate position on circular path with gentle vertical bobbing
    float bob = sin(m_angle * 3.0f) * 0.5f; // subtle up/down
    m_position = vec3(m_center.x + m_orbitRadius * cos(m_angle), m_height + bob,
        m_center.z + m_orbitRadius * sin(m_angle));

    // Advance animation time
    m_animTime += dt;
}

void Bird::draw(GLuint modelMatrixLocation) const {
    if (!m_frames || m_frameCount <= 0)
        return;

    // Pick the current animation frame
    int frame = (int)(m_animTime * m_animSpeed) % m_frameCount;
    if (frame < 0)
        frame = 0;

    Drawable* currentFrame = m_frames[frame];
    if (!currentFrame)
        return;

    // Build model matrix
    mat4 M(1.0f);

    // 1. Translate to world position
    M = translate(M, m_position);

    // 2. Rotate to face direction of travel (tangent to circle)
    vec3 tangent = vec3(-sin(m_angle), 0.0f, cos(m_angle));
    float facingAngle = atan2(tangent.x, tangent.z);
    M = rotate(M, facingAngle, vec3(0, 1, 0));

    // 3. Scale the bird model
    M = scale(M, vec3(m_scale));

    // Upload model matrix and draw
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);
    currentFrame->bind();
    currentFrame->draw();
}
