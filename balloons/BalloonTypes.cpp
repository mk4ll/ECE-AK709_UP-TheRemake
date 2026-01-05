#include "balloonTypes.h"

struct Material {
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;
    float Ns;
};

// get balloon color
glm::vec3 getBalloonColor(BalloonType type) {
    switch (type) {
    case BalloonType::CLASSIC:
        return glm::vec3(0.85f, 0.10f, 0.10f); // red

    case BalloonType::GLITTER:
        return glm::vec3(0.95f, 0.75f, 0.80f); // pink with glitter

    case BalloonType::METALLIC:
        return glm::vec3(0.8f, 0.8f, 0.9f); // silver

    case BalloonType::NEON:
        return glm::vec3(0.1f, 0.9f, 0.3f); // green neon

    case BalloonType::TRANSPARENT:
        return glm::vec3(0.7f, 0.85f, 0.95f); // blue transparent (low opacity)

    case BalloonType::TEXTURED_3D:
        return glm::vec3(0.9f, 0.5f, 0.2f); // orange

    default:
        return glm::vec3(0.85f, 0.10f, 0.10f);
    }
}

//get balloon material
Material getBalloonMaterial(BalloonType type) {
    switch (type) {
    case BalloonType::CLASSIC:
        return Material{
            vec4(0.12f, 0.02f, 0.02f, 1.0f),  // Ka
            vec4(0.85f, 0.10f, 0.10f, 1.0f),  // Kd - red
            vec4(0.35f, 0.35f, 0.35f, 1.0f),  // Ks
            48.0f                                  // Ns - medium shine
        };

    case BalloonType::GLITTER:
        return Material{
            glm::vec4(0.35f, 0.25f, 0.30f, 1.0f),  // Ka
            glm::vec4(0.95f, 0.75f, 0.85f, 1.0f),  // Kd - ροζ
            glm::vec4(0.95f, 0.95f, 0.95f, 1.0f),  // Ks - shiny for sparkles
            128.0f                                 // Ns - ultra glossy
        };

    case BalloonType::METALLIC:
        return Material{
            glm::vec4(0.3f, 0.3f, 0.35f, 1.0f),    // Ka
            glm::vec4(0.6f, 0.6f, 0.7f, 1.0f),     // Kd - metallic gray
            glm::vec4(0.9f, 0.9f, 0.95f, 1.0f),    // Ks - mirror-like
            256.0f                                 // Ns - chrome effect
        };

    case BalloonType::NEON:
        return Material{
            glm::vec4(0.5f, 0.9f, 0.6f, 1.0f),     // Ka - emissive
            glm::vec4(0.1f, 0.9f, 0.3f, 1.0f),     // Kd - neon green
            glm::vec4(0.8f, 1.0f, 0.8f, 1.0f),     // Ks - glow
            64.0f
        };

    case BalloonType::TRANSPARENT:
        return Material{
            glm::vec4(0.1f, 0.15f, 0.2f, 0.3f),    // Ka - διάφανο!
            glm::vec4(0.7f, 0.85f, 0.95f, 0.3f),   // Kd - γαλάζιο με alpha
            glm::vec4(0.9f, 0.9f, 1.0f, 1.0f),     // Ks - glass-like
            96.0f
        };

    case BalloonType::TEXTURED_3D:
        return Material{
            glm::vec4(0.2f, 0.1f, 0.05f, 1.0f),    // Ka
            glm::vec4(0.9f, 0.5f, 0.2f, 1.0f),     // Kd - πορτοκαλί
            glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),     // Ks
            32.0f
        };

    default:
        return Material{
            glm::vec4(0.12f, 0.02f, 0.02f, 1.0f),
            glm::vec4(0.85f, 0.10f, 0.10f, 1.0f),
            glm::vec4(0.35f, 0.35f, 0.35f, 1.0f),
            48.0f
        };
    }
}


// get shader flag
// glUniform1i(useTextureLocation, flag)
int getBalloonShaderFlag(BalloonType type) {
    switch (type) {
    case BalloonType::CLASSIC:     return 3;
    case BalloonType::GLITTER:     return 4;
    case BalloonType::METALLIC:    return 5;
    case BalloonType::NEON:        return 6;
    case BalloonType::TRANSPARENT: return 7;
    case BalloonType::TEXTURED_3D: return 8;
    default:                       return 3;
    }
}

// get type name (for debugging)
const char* getBalloonTypeName(BalloonType type) {
    switch (type) {
    case BalloonType::CLASSIC:     return "Classic";
    case BalloonType::GLITTER:     return "Glitter";
    case BalloonType::METALLIC:    return "Metallic";
    case BalloonType::NEON:        return "Neon";
    case BalloonType::TRANSPARENT: return "Transparent";
    case BalloonType::TEXTURED_3D: return "Textured 3D";
    default:                       return "Unknown";
    }
}


// cycle through types based on index
BalloonType getBalloonTypeByIndex(int index) {
    // 6 indicies
    switch (index % 6) {
    case 0: return BalloonType::CLASSIC;
    case 1: return BalloonType::GLITTER;
    case 2: return BalloonType::METALLIC;
    case 3: return BalloonType::NEON;
    case 4: return BalloonType::TRANSPARENT;
    case 5: return BalloonType::TEXTURED_3D;
    default: return BalloonType::CLASSIC;
    }
}