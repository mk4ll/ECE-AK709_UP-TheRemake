#include "beacon.h"
#include <terrain/terrain.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <random>

using namespace glm;

Beacon::Beacon(const vec3& position, float radius, float height)
    : m_position(position),
    m_radius(radius),
    m_height(height),
    m_mesh(nullptr),
    m_animationTime(0.0f),
    m_animationSpeed(1.0f)
{
    generateCylinderMesh(16, 8);  // Higher resolution for better pattern display
}

Beacon::~Beacon() {
    if (m_mesh) {
        delete m_mesh;
    }
}

void Beacon::generateCylinderMesh(int radialSegments, int heightSegments) {
    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;

    // Create cylinder with proper UVs for texture animation
    for (int h = 0; h < heightSegments; ++h) {
        float y0 = (float)h / heightSegments;
        float y1 = (float)(h + 1) / heightSegments;

        for (int i = 0; i < radialSegments; ++i) {
            float t0 = (float)i / radialSegments;
            float t1 = (float)(i + 1) / radialSegments;

            float angle0 = t0 * two_pi<float>();
            float angle1 = t1 * two_pi<float>();

            // Vertices at different heights
            vec3 p00(cos(angle0) * m_radius, y0 * m_height, sin(angle0) * m_radius);
            vec3 p01(cos(angle1) * m_radius, y0 * m_height, sin(angle1) * m_radius);
            vec3 p10(cos(angle0) * m_radius, y1 * m_height, sin(angle0) * m_radius);
            vec3 p11(cos(angle1) * m_radius, y1 * m_height, sin(angle1) * m_radius);

            // Two triangles per quad
            vertices.push_back(p00);
            vertices.push_back(p10);
            vertices.push_back(p11);

            vertices.push_back(p00);
            vertices.push_back(p11);
            vertices.push_back(p01);

            // UVs - these will be used for the animated patterns in the shader
            // U wraps around the cylinder (0 to 1)
            // V goes from bottom to top (0 to 1)
            uvs.push_back(vec2(t0, y0));
            uvs.push_back(vec2(t0, y1));
            uvs.push_back(vec2(t1, y1));

            uvs.push_back(vec2(t0, y0));
            uvs.push_back(vec2(t1, y1));
            uvs.push_back(vec2(t1, y0));

            // Normals (pointing outward)
            vec3 n0 = normalize(vec3(cos(angle0), 0.0f, sin(angle0)));
            vec3 n1 = normalize(vec3(cos(angle1), 0.0f, sin(angle1)));

            normals.push_back(n0);
            normals.push_back(n0);
            normals.push_back(n1);

            normals.push_back(n0);
            normals.push_back(n1);
            normals.push_back(n1);
        }
    }

    m_mesh = new Drawable(vertices, uvs, normals);
}

void Beacon::update(float dt) {
    m_animationTime += dt * m_animationSpeed;

    // Keep animation time in reasonable range to avoid floating point issues
    if (m_animationTime > 100.0f) {
        m_animationTime -= 100.0f;
    }
}

void Beacon::draw(GLuint modelMatrixLocation, GLuint timeLocation) const {
    mat4 M(1.0f);
    M = translate(M, m_position);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &M[0][0]);

    // Pass animation time to shader for surface patterns
    if (timeLocation != (GLuint)-1) {
        glUniform1f(timeLocation, m_animationTime);
    }

    m_mesh->bind();
    m_mesh->draw();
}

vec3 Beacon::generateRandomBeaconPosition(float terrainSize, float maxHeight) {
    // Use random device for better randomness
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define the right tepui area (positive X side)
    // The right plateau is centered around x = +0.30 * terrainSize
    float rightPlateauCenter = 0.30f * terrainSize;
    float plateauWidth = 0.14f * terrainSize;  // Based on terrain.cpp plateau definition

    // Random position within the right plateau
    std::uniform_real_distribution<float> xDist(
        rightPlateauCenter - plateauWidth * 0.8f,
        rightPlateauCenter + plateauWidth * 0.8f
    );

    // Random Z position (avoiding edges)
    std::uniform_real_distribution<float> zDist(
        -terrainSize * 0.4f,
        terrainSize * 0.4f
    );

    float x = xDist(gen);
    float z = zDist(gen);

    // Get the terrain height at this position
    float terrainHeight = Terrain::sampleHeight(x, z, terrainSize, maxHeight);

    // Place beacon slightly above terrain (starting from ground level)
    float y = terrainHeight;

    return vec3(x, y, z);
}