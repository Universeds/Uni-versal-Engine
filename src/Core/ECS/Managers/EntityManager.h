#pragma once
#include <cstdint>
#include <queue>
#include <vector>
#include <set>
#include "../Entity.h"
#include "../Component.h"

namespace UniversalEngine {
    
    using Signature = std::set<ComponentTypeID>;
    
    class EntityManager {
    public:
        EntityManager();
        ~EntityManager() = default;
        
        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;
        
        EntityManager(EntityManager&&) = default;
        EntityManager& operator=(EntityManager&&) = default;
        
        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        bool IsEntityValid(Entity entity) const;
        
        void SetSignature(Entity entity, const Signature& signature);
        Signature GetSignature(Entity entity) const;
        
        size_t GetLivingEntityCount() const { return m_LivingEntityCount; }
        
    private:
        std::queue<EntityID> m_AvailableEntities;
        
        std::vector<Signature> m_Signatures;
        
        EntityID m_NextEntityID;
        
        size_t m_LivingEntityCount;
    };
    
}
