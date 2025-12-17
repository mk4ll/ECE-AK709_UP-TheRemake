#pragma once
#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    Terrain(const char* heightmapPath,
        float heightScale,
        float size);

    void Draw();

private:
    unsigned int VAO, VBO, EBO;
    int width, height;

    void loadHeightmap(const char* path,
        float heightScale,
        float size);
};
