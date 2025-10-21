#pragma once
#include "../ECS/Component.h"
#include <glm/glm.hpp>
#include <vector>

namespace UniversalEngine {
    
    class MeshRenderer2D : public Component {
    public:
        std::vector<float> vertices;
        
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
        
        glm::vec2 size{1.0f, 1.0f};
        
        bool visible{true};
        
        MeshRenderer2D() {
            vertices = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.5f,  0.5f,
                -0.5f,  0.5f
            };
        }
        
        MeshRenderer2D(const glm::vec2& quadSize, const glm::vec4& quadColor = glm::vec4(1.0f)) 
            : size(quadSize), color(quadColor) {
            float halfWidth = quadSize.x * 0.5f;
            float halfHeight = quadSize.y * 0.5f;
            
            vertices = {
                -halfWidth, -halfHeight,
                 halfWidth, -halfHeight,
                 halfWidth,  halfHeight,
                -halfWidth,  halfHeight
            };
        }
    };
    
}
