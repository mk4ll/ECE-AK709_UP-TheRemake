#ifndef RIVER_H
#define RIVER_H

#include <vector>
#include <glm/glm.hpp>
#include <common/model.h> // for Drawable

class River {
public:
    // Creates a river mesh from a path (center line)
    static Drawable* createRiver(const std::vector<glm::vec3>& path, float width);
};

#endif
