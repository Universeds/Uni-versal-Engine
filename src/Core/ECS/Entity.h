#pragma once
#include <cstdint>

namespace UniversalEngine {
    
    using EntityID = std::uint32_t;
    
    constexpr EntityID INVALID_ENTITY = 0;
    
    class Entity {
    public:
        Entity() : m_ID(INVALID_ENTITY) {}
        explicit Entity(EntityID id) : m_ID(id) {}
        
        EntityID GetID() const { return m_ID; }
        bool IsValid() const { return m_ID != INVALID_ENTITY; }
        
        bool operator==(const Entity& other) const { return m_ID == other.m_ID; }
        bool operator!=(const Entity& other) const { return m_ID != other.m_ID; }
        bool operator<(const Entity& other) const { return m_ID < other.m_ID; }
        
        operator EntityID() const { return m_ID; }
        
    private:
        EntityID m_ID;
    };
    
    class World;
    
}
