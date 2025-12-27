#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glm/glm.hpp>
#include <common/model.h> // where Drawable is defined

class Terrain {
public:
    // Parameters: width/depth size, resolution (grid density) and mountain height
    static Drawable* generate(float size, int resolution, float maxHeight);

    // calculate the peak position
    static glm::vec3 get_terrain_peak();

    // generate river
    static std::vector<glm::vec3> generateRiverPath(
        float size,
        int resolution,
        float maxHeight
    );

private:
    static float getHeight(float x, float z, float size, float maxHeight);
    // static var to store the peak pos
    static glm::vec3 peakPos;
};

#endif