#include "river.h"

using namespace glm;
using namespace std;

Drawable* River::createRiver(const vector<vec3>& path, float width) {

    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;

    for (size_t i = 0; i < path.size() - 1; i++) {

        vec3 p0 = path[i];
        vec3 p1 = path[i + 1];

        // direction of river segment
        vec3 dir = normalize(p1 - p0);

        // perpendicular direction
        vec3 side = normalize(cross(dir, vec3(0, 1, 0)));

        // left and right points
        vec3 L0 = p0 + side * width;
        vec3 R0 = p0 - side * width;
        vec3 L1 = p1 + side * width;
        vec3 R1 = p1 - side * width;

        // two triangles
        vertices.insert(vertices.end(), {
            L0, R0, L1,
            R0, R1, L1
            });

        // simple UVs
        uvs.insert(uvs.end(), {
            {0,0}, {1,0}, {0,1},
            {1,0}, {1,1}, {0,1}
            });

        // flat normals (upwards)
        for (int k = 0; k < 6; k++)
            normals.push_back(vec3(0.0f, 1.0f, 0.0f));
    }

    return new Drawable(vertices, uvs, normals);
}
