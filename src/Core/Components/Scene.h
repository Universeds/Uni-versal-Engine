#pragma once
#include "../ECS/Component.h"

namespace UniversalEngine {
    
    class Scene : public Component {
    public:
        float timeScale{1.0f};
        
        Scene() = default;
        Scene(float scale) : timeScale(scale) {}
        
        float GetScaledDeltaTime(float deltaTime) const {
            return deltaTime * timeScale;
        }
    };
    
}
