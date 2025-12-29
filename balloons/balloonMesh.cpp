#include "balloonMesh.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

BalloonMesh::BalloonMesh() {

    const int heightSegments = 20;
    const int radialSegments = 20;
    const float height = 2.0f;
    const float radius = 1.0f;

    // vertices
    for (int i = 0; i <= heightSegments; i++) {

        float v = (float)i / heightSegments;
        float y = -height * 0.5f + v * height;

        // balloon profile (how inflated it is)
        float profile = sin(v * glm::pi<float>());
        float r = radius * profile;

        for (int j = 0; j <= radialSegments; j++) {

            float u = (float)j / radialSegments;
            float angle = u * glm::two_pi<float>();

            float x = r * cos(angle);
            float z = r * sin(angle);

            glm::vec3 position(x, y, z);
            glm::vec3 normal = glm::normalize(position);
            glm::vec2 uv(u, v);

            vertices.push_back({ position, normal, uv });
        }
    }

    // indices
    int ringSize = radialSegments + 1;

    for (int i = 0; i < heightSegments; i++) {
        for (int j = 0; j < radialSegments; j++) {

            int A = i * ringSize + j;
            int B = A + 1;
            int C = A + ringSize;
            int D = C + 1;

            // triangle 1
            indices.push_back(A);
            indices.push_back(C);
            indices.push_back(B);

            // triangle 2
            indices.push_back(B);
            indices.push_back(C);
            indices.push_back(D);
        }
    }
}
