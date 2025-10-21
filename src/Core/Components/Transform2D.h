#pragma once
#include "../ECS/Component.h"
#include <glm/glm.hpp>

namespace UniversalEngine {
    class Transform2D : public Component {
    public:
        glm::vec2 scale{1.0f};
        glm::vec2 position{0.0f};
        float rotation{0.0f};
        
        Transform2D() = default;
        Transform2D(const glm::vec2& pos, const glm::vec2& scl = glm::vec2(1.0f), float rot = 0.0f)
            : position(pos), scale(scl), rotation(rot) {}
    };
}
