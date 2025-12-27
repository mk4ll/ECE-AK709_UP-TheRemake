#ifndef RIVER_H
#define RIVER_H

#include <vector>
#include <glm/glm.hpp>
#include <common/model.h> // for Drawable

class River {
public:
    // Creates a river mesh from a path (center line)
    static Drawable* createFloodedCanyon(float size, int resolution, float waterLevel, float maxHeight);
};

#endif
