#include "balloonMesh.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

BalloonMesh::BalloonMesh(int heightSegments, int radialSegments, float height, float radius) {
    generate(heightSegments, radialSegments, height, radius);
}

// curve that needs to be revoluted r(y):[0,H]-->[0:R]
float BalloonMesh::radiusAt(float y, float H, float R){

    float h_body = H - R;

    // body
    if (y <= h_body) {
        float r_body = y * y;
        float scale = R / (h_body * h_body);
        return r_body * scale;
    }

    // sphere
    float yc = h_body;
    float dy = y - yc;
    return std::sqrt(glm::max(0.0f, R * R - dy * dy));
}


void BalloonMesh::generate(int hSeg, int rSeg, float H, float R) {
    positions.clear();
    uvs.clear();

    for (int i = 0; i < hSeg; ++i) {
        float t0 = (float)i / hSeg;
        float t1 = (float)(i + 1) / hSeg;

        float y0 = H * t0;
        float y1 = H * t1;

        float r0 = radiusAt(y0, H, R);
        float r1 = radiusAt(y1, H, R);

        for (int j = 0; j < rSeg; ++j) {
            float u0 = (float)j / rSeg;
            float u1 = (float)(j + 1) / rSeg;

            float th0 = u0 * glm::two_pi<float>();
            float th1 = u1 * glm::two_pi<float>();

            // quad vertices
            glm::vec3 p00(r0 * cos(th0), y0, r0 * sin(th0));
            glm::vec3 p10(r1 * cos(th0), y1, r1 * sin(th0));
            glm::vec3 p11(r1 * cos(th1), y1, r1 * sin(th1));
            glm::vec3 p01(r0 * cos(th1), y0, r0 * sin(th1));

            glm::vec2 uv00(u0, t0);
            glm::vec2 uv10(u0, t1);
            glm::vec2 uv11(u1, t1);
            glm::vec2 uv01(u1, t0);

            // triangle 1
            positions.push_back(p00);
            positions.push_back(p10);
            positions.push_back(p11);

            uvs.push_back(uv00);
            uvs.push_back(uv10);
            uvs.push_back(uv11);

            // triangle 2
            positions.push_back(p00);
            positions.push_back(p11);
            positions.push_back(p01);

            uvs.push_back(uv00);
            uvs.push_back(uv11);
            uvs.push_back(uv01);
        }
    }
}