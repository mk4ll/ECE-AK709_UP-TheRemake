#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <common/vertex.h>

class BalloonMesh {
public:
	// vertices (pos, normal, uv)
	std::vector<Vertex> vertices;
	// 3indices = 1triangle
	std::vector<unsigned int> indices;

	BalloonMesh();
};