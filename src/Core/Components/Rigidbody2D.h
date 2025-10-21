#pragma once
#include "../ECS/Component.h"
#include <glm/glm.hpp>

namespace UniversalEngine {
    
    class Rigidbody2D : public Component {
    public:
        glm::vec2 velocity{0.0f, 0.0f};
        
        float angularVelocity{0.0f};
        
        float gravityScale{1.0f};
        
        float mass{1.0f};
        
        float restitution{0.3f};
        
        float friction{0.3f};
        
        float drag{0.01f};
        
        float angularDrag{0.05f};
        
        bool useGravity{true};
        
        Rigidbody2D() = default;
        
        Rigidbody2D(float gravScale, float m = 1.0f, bool gravity = true, float rest = 0.3f, float fric = 0.3f)
            : gravityScale(gravScale), mass(m), useGravity(gravity), restitution(rest), friction(fric) {}
        
        float GetInertia(const glm::vec2& size) const {
            return (mass / 12.0f) * (size.x * size.x + size.y * size.y);
        }
    };
    
}
