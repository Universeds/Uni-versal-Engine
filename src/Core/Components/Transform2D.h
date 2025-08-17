#pragma once
#include <glm/glm.hpp>

namespace UniversalEngine {
    struct Transform2D {
        glm::vec2 scale{1.0f};
        glm::vec2 position{0.0f};
        int rotation{0.0f}; 
    };
}