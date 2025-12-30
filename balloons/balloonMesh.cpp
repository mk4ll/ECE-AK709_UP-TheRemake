#include "balloonMesh.h"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

BalloonMesh::BalloonMesh(int heightSegments, int radialSegments, float height, float radius) {
    generate(heightSegments, radialSegments, height, radius);
}

// curve that needs to be revoluted r(y):[0,H]-->[0:R]
float BalloonMesh::radiusAt(float y, float H, float R){
    
    float r_knot = 0.15f * R;
    float h_knot = 0.08f * H;
    float h_body = H - R;

    // knot
    if (y <= h_knot) {
        float t = y / h_knot;      // normalization [0,1]
        return r_knot * (1.1 - t);
    }

    // body
    if (y <= h_body) {
        float yb = y - h_knot;
        float hb = h_body - h_knot;

        float alpha = 0.5f;
        float r_body = std::pow(yb, alpha);
        float scale = R / std::pow(hb, alpha);
        return r_body * scale;
    }

    // top (hemisphere)
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

        // for numerical approximation of normals
        float eps = 0.001f;
        float drdy0 = (radiusAt(y0 + eps, H, R) - radiusAt(y0 - eps, H, R)) / (2.0f * eps);
        float drdy1 = (radiusAt(y1 + eps, H, R) - radiusAt(y1 - eps, H, R)) / (2.0f * eps);



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

            glm::vec3 n00 = glm::normalize(glm::vec3(cos(th0), -drdy0, sin(th0)));
            glm::vec3 n10 = glm::normalize(glm::vec3(cos(th0), -drdy1, sin(th0)));
            glm::vec3 n11 = glm::normalize(glm::vec3(cos(th1), -drdy1, sin(th1)));
            glm::vec3 n01 = glm::normalize(glm::vec3(cos(th1), -drdy0, sin(th1)));


            // triangle 1
            positions.push_back(p00); normals.push_back(n00);
            positions.push_back(p10); normals.push_back(n10);
            positions.push_back(p11); normals.push_back(n11);

            uvs.push_back(uv00);
            uvs.push_back(uv10);
            uvs.push_back(uv11);

            // triangle 2
            positions.push_back(p00); normals.push_back(n00);
            positions.push_back(p11); normals.push_back(n11);
            positions.push_back(p01); normals.push_back(n01);

            uvs.push_back(uv00);
            uvs.push_back(uv11);
            uvs.push_back(uv01);
        }
    }
}