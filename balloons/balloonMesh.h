#pragma once

#include <vector>
#include <glm/glm.hpp>

class BalloonMesh {
public:
	// vertices (pos, normal, uv)
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;

	BalloonMesh(int heightSegments = 64, 
				int radialSegments = 64, 
				float height = 3.0f, 
				float radius = 1.0f);
private:
	float radiusAt(float y,
				   float H,
				   float R);

	void generate(int heightSegments,
		   		  int radialSegments,
				  float height,
				  float radius);
};