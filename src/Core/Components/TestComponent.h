#pragma once
#include "../ECS/Component.h"

namespace UniversalEngine {
    
    class TestComponent : public Component {
    public:
        TestComponent(int value = 0) : m_Value(value) {}
        
        int GetValue() const { return m_Value; }
        void SetValue(int value) { m_Value = value; }
        
    private:
        int m_Value;
    };
    
}
