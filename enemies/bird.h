#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Drawable; // forward declare

class Bird {
public:
    // frames = shared array of Drawable* (one per animation frame)
    // frameCount = number of frames (e.g. 21)
    // center = center of the circular flight path
    // orbitRadius = radius of the circular orbit
    // speed = angular speed in radians/sec
    // height = flight altitude (Y)
    Bird(Drawable** frames, int frameCount, glm::vec3 center, float orbitRadius,
        float speed, float height, float startAngle = 0.0f);

    void update(float dt);
    void draw(GLuint modelMatrixLocation) const;

    glm::vec3 getPosition() const { return m_position; }
    float getCollisionRadius() const { return m_collisionRadius; }

    // Allow updating the orbit center at runtime (to track house)
    void setOrbitCenter(glm::vec3 center) { m_center = center; }
    void setHeight(float h) { m_height = h; }

private:
    // Animation
    Drawable** m_frames;
    int m_frameCount;
    float m_animTime;
    float m_animSpeed; // frames per second

    // Flight path (circular orbit)
    glm::vec3 m_center;
    float m_orbitRadius;
    float m_speed;
    float m_angle;
    float m_height;

    // State
    glm::vec3 m_position;
    float m_collisionRadius;
    float m_scale;
};
