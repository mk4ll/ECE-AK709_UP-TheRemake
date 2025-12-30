#include "rope.h"
#include <vector>
#include <cmath>
#include <glm/gtc/constants.hpp>

using namespace glm;
using namespace std;

Drawable* Rope::create(
    const vec3& origin,
    const vec3& end,
    int radialSegments,
    float radius
) {
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;

    vec3 axis = end - origin;
    float height = length(axis);
    vec3 dir = normalize(axis);

    // create arbitrary perpendicular basis
    vec3 up = abs(dot(dir, vec3(0, 1, 0))) > 0.9f ? vec3(1, 0, 0) : vec3(0, 1, 0);
    vec3 right = normalize(cross(dir, up));
    vec3 forward = cross(right, dir);

    for (int i = 0; i < radialSegments; ++i) {
        float t0 = float(i) / radialSegments;
        float t1 = float(i + 1) / radialSegments;

        float a0 = t0 * two_pi<float>();
        float a1 = t1 * two_pi<float>();

        vec3 r0 = radius * (cos(a0) * right + sin(a0) * forward);
        vec3 r1 = radius * (cos(a1) * right + sin(a1) * forward);

        vec3 p00 = origin + r0;
        vec3 p01 = origin + r1;
        vec3 p10 = end + r0;
        vec3 p11 = end + r1;

        // triangle 1
        vertices.push_back(p00);
        vertices.push_back(p10);
        vertices.push_back(p11);

        // triangle 2
        vertices.push_back(p00);
        vertices.push_back(p11);
        vertices.push_back(p01);

        // UVs (cylindrical mapping)
        uvs.push_back(vec2(t0, 0));
        uvs.push_back(vec2(t0, 1));
        uvs.push_back(vec2(t1, 1));

        uvs.push_back(vec2(t0, 0));
        uvs.push_back(vec2(t1, 1));
        uvs.push_back(vec2(t1, 0));

        // flat normals
        vec3 n0 = normalize(r0);
        vec3 n1 = normalize(r1);

        normals.push_back(n0);
        normals.push_back(n0);
        normals.push_back(n1);

        normals.push_back(n0);
        normals.push_back(n1);
        normals.push_back(n1);
    }

    return new Drawable(vertices, uvs, normals);
}
