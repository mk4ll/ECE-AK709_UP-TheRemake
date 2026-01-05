#pragma once

#include <glm/glm.hpp>

using namespace glm;

// ====== BALLOON TYPE DEFINITIONS ====== //

enum class BalloonType {
    CLASSIC,
    GLITTER,
    METALLIC,
    NEON,
    TRANSPARENT,
    TEXTURED_3D
};

struct Material;

vec3 getBalloonColor(BalloonType type);

// get material (Ka, Kd, Ks, Ns)
Material getBalloonMaterial(BalloonType type);

// get shader flag (useTexture value) για κάθε τύπο
int getBalloonShaderFlag(BalloonType type);

// (for debugging)
const char* getBalloonTypeName(BalloonType type);

// helper func: cycle through types
BalloonType getBalloonTypeByIndex(int index);