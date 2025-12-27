#include "terrain.h"
#include <cmath>

using namespace glm;
using namespace std;

// initialize the static var for the peak
glm::vec3 Terrain::peakPos = glm::vec3(0.0f);

float plateau(float x, float center, float topWidth, float cliffWidth) {
    float d = abs(x - center);
    if (d < topWidth) return 1.0f;
    float t = (d - topWidth) / cliffWidth;
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - t * t * (3.0f - 2.0f * t);
}

float Terrain::getHeight(float x, float z, float size, float maxHeight) {

    float nx = x / size;
    float nz = z / size;

    // edge wobble
    float edgeWobble = sin(nz * 6.0f) * 0.035f
        + sin(nz * 13.0f) * 0.015f;
    float wx = nx + edgeWobble;

    float left = plateau(wx, -0.30f, 0.10f, 0.015f);
    float right = plateau(wx, 0.30f, 0.14f, 0.015f);

    float height =
        left * maxHeight * 0.55f +
        right * maxHeight * 1.15f;

    // canyon
    float meander = sin(nz * 16.0f) * 0.025f;
    float sx = nx + meander;

    height -= exp(-pow(sx / 0.04f, 2.0f)) * maxHeight;
    height += exp(-pow(sx / 0.012f, 2.0f)) * maxHeight * 0.12f;

    // masks
    float topMask = glm::max(left, right);
    float edgeMask = 1.0f - smoothstep(0.7f, 1.0f, topMask);

    // rocky cliffs
    float cliffNoise = 0.0f;
    cliffNoise += sin(height * 5.0f + z * 0.4f) * 0.20f;
    cliffNoise += sin(height * 17.0f + nx * 12.0f) * 0.08f;

    float slopeMask = smoothstep(0.2f, 0.6f, edgeMask);

    return height + cliffNoise * slopeMask;
}


glm::vec3 Terrain::get_terrain_peak() {
    return peakPos;
}

Drawable* Terrain::generate(float size, int resolution, float maxHeight) {
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;

    float step = size / (float)resolution;
    float offset = size / 2.0f;
    //reset for new gen
    float currentMaxY = -1e9f;

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

            // lists thata store heights and according positions
            float heights[] = { h11, h12, h21, h22 };
            vec3 positions[] = { {x1, h11, z1}, {x1, h12, z2}, {x2, h21, z1}, {x2, h22, z2} };
            
            // terrain peak calculation
            for (int k = 0; k < 4; ++k) {
                if (heights[k] > currentMaxY) {
                    currentMaxY = heights[k];
                    peakPos = positions[k];
                }
            }

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

// river

// helper function to read the terrain
float Terrain::sampleHeight(float x, float z, float size, float maxHeight) {
    return getHeight(x, z, size, maxHeight);
}

