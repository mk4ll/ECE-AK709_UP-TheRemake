#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <common/model.h>

class Beacon {
public:
    Beacon(const glm::vec3& position, float radius = 5.0f, float height = 20.0f);
    ~Beacon();

    // Rendering
    void draw(GLuint modelMatrixLocation, GLuint timeLocation) const;
    void update(float dt);

    // Getters
    const glm::vec3& getPosition() const { return m_position; }
    float getRadius() const { return m_radius; }
    float getHeight() const { return m_height; }
    float getAnimationTime() const { return m_animationTime; }

    // Generate a random position on the right tepui (opposite from house)
    static glm::vec3 generateRandomBeaconPosition(float terrainSize, float maxHeight, const glm::vec3& housePosition);

private:
    void generateCylinderMesh(int radialSegments, int heightSegments);

    glm::vec3 m_position;
    float m_radius;
    float m_height;

    Drawable* m_mesh;

    // Animation time for shader-based surface patterns
    float m_animationTime;
    float m_animationSpeed;
};