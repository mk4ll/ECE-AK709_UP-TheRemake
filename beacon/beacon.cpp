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
    generateCylinderMesh(16, 8);
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

vec3 Beacon::generateRandomBeaconPosition(float terrainSize, float maxHeight, const vec3& housePosition) {
    // Use random device for better randomness
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Tepui centers from terrain.cpp
    float leftPlateauCenter = -0.30f * terrainSize;
    float rightPlateauCenter = 0.30f * terrainSize;

    // Determine which tepui the house is on by checking its X coordinate
    bool houseOnLeftTepui = (housePosition.x < 0.0f);

    // Choose the opposite tepui for beacon
    float targetPlateauCenter;
    float plateauWidth;

    if (houseOnLeftTepui) {
        // House on left => spawn beacon on RIGHT tepui
        targetPlateauCenter = rightPlateauCenter;
        plateauWidth = 0.14f * terrainSize;  // Right plateau width
    }
    else {
        // House on right => spawn beacon on LEFT tepui  
        targetPlateauCenter = leftPlateauCenter;
        plateauWidth = 0.10f * terrainSize;  // Left plateau width
    }

    // Random position within the target plateau
    std::uniform_real_distribution<float> xDist(
        targetPlateauCenter - plateauWidth * 0.8f,
        targetPlateauCenter + plateauWidth * 0.8f
    );

    // Random Z position (avoiding edges)
    std::uniform_real_distribution<float> zDist(
        -terrainSize * 0.4f,
        terrainSize * 0.4f
    );

    // REJECTION SAMPLING: Try random positions until we find one with acceptable slope
    const float MAX_SLOPE = 0.15f;  // Maximum acceptable slope (tan of angle)
    const int MAX_ATTEMPTS = 100;    // Safety limit to avoid infinite loop

    float x, z, terrainHeight;
    int attempts = 0;
    bool foundGoodSpot = false;

    while (!foundGoodSpot && attempts < MAX_ATTEMPTS) {
        x = xDist(gen);
        z = zDist(gen);

        // Get the terrain height at this position
        terrainHeight = Terrain::sampleHeight(x, z, terrainSize, maxHeight);

        // Calculate slope by sampling nearby points
        const float sampleDist = 0.5f;  // Distance to sample for gradient calculation

        float heightX1 = Terrain::sampleHeight(x - sampleDist, z, terrainSize, maxHeight);
        float heightX2 = Terrain::sampleHeight(x + sampleDist, z, terrainSize, maxHeight);
        float heightZ1 = Terrain::sampleHeight(x, z - sampleDist, terrainSize, maxHeight);
        float heightZ2 = Terrain::sampleHeight(x, z + sampleDist, terrainSize, maxHeight);

        // Calculate gradients
        float gradientX = (heightX2 - heightX1) / (2.0f * sampleDist);
        float gradientZ = (heightZ2 - heightZ1) / (2.0f * sampleDist);

        // Calculate slope magnitude (tan of the angle)
        float slope = sqrt(gradientX * gradientX + gradientZ * gradientZ);

        // Check if slope is acceptable
        if (slope < MAX_SLOPE) {
            foundGoodSpot = true;
        }

        attempts++;
    }

    // If we couldn't find a good spot after many attempts, use the last one anyway
    // won't happen, because we have plenty of flat surfaces, but just in case
    if (!foundGoodSpot) {
        printf("Warning: Could not find flat spot for beacon after %d attempts. Using last position.\n", MAX_ATTEMPTS);
    }

    // Place beacon slightly above terrain (starting from ground level)
    float y = terrainHeight;

    return vec3(x, y, z);
}