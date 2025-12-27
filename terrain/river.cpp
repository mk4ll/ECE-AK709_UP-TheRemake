#include "river.h"
#include "terrain.h"

using namespace glm;
using namespace std;

Drawable* River::createFloodedCanyon(float size, int resolution, float waterLevel, float maxHeight) {

	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;

	float step = size / resolution;
	float offset = size / 2.0f;

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {

            float x1 = i * step - offset;
            float z1 = j * step - offset;
            float x2 = (i + 1) * step - offset;
            float z2 = (j + 1) * step - offset;

            float h11 = Terrain::sampleHeight(x1, z1, size, maxHeight);
            float h12 = Terrain::sampleHeight(x1, z2, size, maxHeight);
            float h21 = Terrain::sampleHeight(x2, z1, size, maxHeight);
            float h22 = Terrain::sampleHeight(x2, z2, size, maxHeight);

            // if over the waterLevel -> no fill
            if (h11 > waterLevel && h12 > waterLevel &&
                h21 > waterLevel && h22 > waterLevel)
                continue;

            vec3 v1{ x1, waterLevel, z1 };
            vec3 v2{ x1, waterLevel, z2 };
            vec3 v3{ x2, waterLevel, z1 };
            vec3 v4{ x2, waterLevel, z2 };

            vertices.insert(vertices.end(), {
                v1, v2, v3,
                v2, v4, v3
                });

            uvs.insert(uvs.end(), {
                {0,0},{0,1},{1,0},
                {0,1},{1,1},{1,0}
                });

            for (int k = 0; k < 6; k++)
                normals.push_back(vec3(0, 1, 0));
        }
    }

    return new Drawable(vertices, uvs, normals);
}