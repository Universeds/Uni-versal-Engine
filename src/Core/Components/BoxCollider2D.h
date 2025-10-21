#pragma once
#include "../ECS/Component.h"
#include <glm/glm.hpp>

namespace UniversalEngine {
    
    class BoxCollider2D : public Component {
    public:
        glm::vec2 size{1.0f, 1.0f};
        
        glm::vec2 offset{0.0f, 0.0f};
        
        bool isTrigger{false};
        
        bool isStatic{false};
        
        BoxCollider2D() = default;
        
        BoxCollider2D(const glm::vec2& colliderSize, bool trigger = false, bool staticCollider = false)
            : size(colliderSize), isTrigger(trigger), isStatic(staticCollider) {}
        
        BoxCollider2D(const glm::vec2& colliderSize, const glm::vec2& colliderOffset, bool trigger = false, bool staticCollider = false)
            : size(colliderSize), offset(colliderOffset), isTrigger(trigger), isStatic(staticCollider) {}
        
        bool Intersects(const BoxCollider2D& other, const glm::vec2& thisPos, const glm::vec2& otherPos) const {
            glm::vec2 thisMin = thisPos + offset - size * 0.5f;
            glm::vec2 thisMax = thisPos + offset + size * 0.5f;
            
            glm::vec2 otherMin = otherPos + other.offset - other.size * 0.5f;
            glm::vec2 otherMax = otherPos + other.offset + other.size * 0.5f;
            
            return (thisMin.x < otherMax.x && thisMax.x > otherMin.x &&
                    thisMin.y < otherMax.y && thisMax.y > otherMin.y);
        }
    };
    
}
