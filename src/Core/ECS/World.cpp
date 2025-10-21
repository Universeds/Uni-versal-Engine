#include "World.h"

namespace UniversalEngine {
    
    World::World() : m_NextEntityID(1), m_LivingEntityCount(0) {
    }
    
    World::~World() {
        Shutdown();
    }
    
    Entity World::CreateEntity() {
        EntityID id;
        
        if (!m_AvailableEntities.empty()) {
            id = m_AvailableEntities.front();
            m_AvailableEntities.pop();
        } else {
            id = m_NextEntityID++;
        }
        
        ++m_LivingEntityCount;
        
        return Entity(id);
    }
    
    void World::DestroyEntity(Entity entity) {
        if (!IsEntityValid(entity)) {
            return;
        }
        
        EntityID entityID = entity.GetID();
        
        m_Signatures[entityID].clear();
        
        for (auto const& pair : m_ComponentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entityID);
        }
        
        for (auto const& pair : m_Systems) {
            auto const& system = pair.second;
            system->RemoveEntity(entity);
        }
        
        m_AvailableEntities.push(entityID);
        --m_LivingEntityCount;
    }
    
    bool World::IsEntityValid(Entity entity) const {
        EntityID entityID = entity.GetID();
        if (entityID == INVALID_ENTITY || entityID >= m_NextEntityID) {
            return false;
        }
        
        // check if entity is in the available entities queue (destroyed)
        std::queue<EntityID> tempQueue = m_AvailableEntities;
        while (!tempQueue.empty()) {
            if (tempQueue.front() == entityID) {
                return false;
            }
            tempQueue.pop();
        }
        
        return true;
    }
    
    void World::ecs_flush() {
        for (auto& operation : m_PendingOperations) {
            operation.operation();
        }
        m_PendingOperations.clear();
    }
    
    void World::Update(float deltaTime) {
        for (auto& system : m_SystemsVector) {
            if (system->IsEnabled()) {
                system->Update(deltaTime);
            }
        }
    }
    
    void World::Render() {
        for (auto& system : m_SystemsVector) {
            if (system->IsEnabled()) {
                system->Render();
            }
        }
    }
    
    void World::Shutdown() {
        for (auto& system : m_SystemsVector) {
            system->Shutdown();
        }
        m_Systems.clear();
        m_SystemsVector.clear();
        m_ComponentArrays.clear();
        m_PendingOperations.clear();
    }
    
    void World::UpdateEntitySystems(Entity entity) {
        EntityID entityID = entity.GetID();
        const Signature& entitySignature = m_Signatures[entityID];
        
        for (auto const& pair : m_Systems) {
            auto const& system = pair.second;
            const Signature& systemSignature = system->GetSignature();
            
            if (SignatureMatches(entitySignature, systemSignature)) {
                system->AddEntity(entity);
            } else {
                system->RemoveEntity(entity);
            }
        }
    }
    
    bool World::SignatureMatches(const Signature& entitySignature, const Signature& systemSignature) const {
        for (ComponentTypeID componentType : systemSignature) {
            if (entitySignature.find(componentType) == entitySignature.end()) {
                return false;
            }
        }
        return true;
    }
    
}
