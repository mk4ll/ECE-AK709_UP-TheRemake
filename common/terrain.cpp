#include "terrain.h"
#include <cmath>

using namespace glm;
using namespace std;

float Terrain::getHeight(float x, float z, float size, float maxHeight) {
    // central mountain using gaussian-like curve
    float dist = sqrt(x * x + z * z);
    float mountain = maxHeight * exp(-pow(dist / (size * 0.3f), 2.0f));

    // simulate basic noise using sine
    float detail = 1.5f * sin(x * 0.5f) * cos(z * 0.5f);

    return mountain + detail;
}

Drawable* Terrain::generate(float size, int resolution, float maxHeight) {
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;

    float step = size / (float)resolution;
    float offset = size / 2.0f;

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {

            // def 4 corners
            float x1 = i * step - offset;
            float z1 = j * step - offset;
            float x2 = (i + 1) * step - offset;
            float z2 = (j + 1) * step - offset;

            // get heights
            float h11 = getHeight(x1, z1, size, maxHeight);
            float h12 = getHeight(x1, z2, size, maxHeight);
            float h21 = getHeight(x2, z1, size, maxHeight);
            float h22 = getHeight(x2, z2, size, maxHeight);

            // triangles for the quad
            vec3 v1{ x1, h11, z1 }, v2{ x1, h12, z2 }, v3{ x2, h21, z1 }, v4{ x2, h22, z2 };

            // triangle 1
            vertices.push_back(v1); vertices.push_back(v2); vertices.push_back(v3);
            // triangle 2
            vertices.push_back(v2); vertices.push_back(v4); vertices.push_back(v3);

            // UV mapping
            uvs.push_back(vec2(0, 0)); uvs.push_back(vec2(0, 1)); uvs.push_back(vec2(1, 0));
            uvs.push_back(vec2(0, 1)); uvs.push_back(vec2(1, 1)); uvs.push_back(vec2(1, 0));

            // normals
            vec3 n1 = normalize(cross(v2 - v1, v3 - v1));
            for (int k = 0; k < 6; k++) normals.push_back(n1);
        }
    }

    return new Drawable(vertices, uvs, normals);
}