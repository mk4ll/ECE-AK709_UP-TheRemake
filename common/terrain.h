#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glm/glm.hpp>
#include <common/model.h> // Assuming this is where Drawable is defined

class Terrain {
public:
    // Parameters: width/depth size, resolution (grid density), and mountain height
    static Drawable* generate(float size, int resolution, float maxHeight);

private:
    static float getHeight(float x, float z, float size, float maxHeight);
};

#endif