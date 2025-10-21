#pragma once
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <typeinfo>
#include <stdexcept>
#include <functional>
#include "Entity.h"
#include "Component.h"
#include "System.h"

namespace UniversalEngine {
    
    using Signature = std::set<ComponentTypeID>;
    
    enum class PendingOperationType {
        ADD_COMPONENT,
        REMOVE_COMPONENT
    };
    
    struct PendingOperation {
        PendingOperationType type;
        Entity entity;
        ComponentTypeID componentType;
        std::function<void()> operation;
    };
    
    class World {
    public:
        World();
        ~World();
        
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        
        World(World&&) = default;
        World& operator=(World&&) = default;
        
        Entity CreateEntity();
        void DestroyEntity(Entity entity);
        bool IsEntityValid(Entity entity) const;
        
        template<typename T>
        void RegisterComponent() {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) != m_ComponentArrays.end()) {
                return;
            }
            
            m_ComponentArrays[typeID] = std::make_unique<ComponentArray<T>>();
        }
        
        template<typename T>
        void AddComponent(Entity entity, T component) {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            if (!IsEntityValid(entity)) {
                throw std::runtime_error("Entity is not valid");
            }
            
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) == m_ComponentArrays.end()) {
                RegisterComponent<T>();
            }
            
            PendingOperation op;
            op.type = PendingOperationType::ADD_COMPONENT;
            op.entity = entity;
            op.componentType = typeID;
            
            // used shared_ptr to make the lambda copy constructible
            auto componentPtr = std::make_shared<T>(std::move(component));
            op.operation = [this, entity, componentPtr, typeID]() {
                this->GetComponentArray<T>()->InsertData(entity.GetID(), std::move(*componentPtr));
                m_Signatures[entity.GetID()].insert(typeID);
                UpdateEntitySystems(entity);
            };
            
            m_PendingOperations.push_back(std::move(op));
        }
        
        template<typename T>
        void RemoveComponent(Entity entity) {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            if (!IsEntityValid(entity)) {
                return;
            }
            
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) == m_ComponentArrays.end()) {
                return;
            }
            
            PendingOperation op;
            op.type = PendingOperationType::REMOVE_COMPONENT;
            op.entity = entity;
            op.componentType = typeID;
            op.operation = [this, entity, typeID]() {
                this->GetComponentArray<T>()->RemoveData(entity.GetID());
                m_Signatures[entity.GetID()].erase(typeID);
                UpdateEntitySystems(entity);
            };
            
            m_PendingOperations.push_back(std::move(op));
        }
        
        template<typename T>
        T& GetComponent(Entity entity) {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            if (!IsEntityValid(entity)) {
                throw std::runtime_error("Entity is not valid");
            }
            
            return GetComponentArray<T>()->GetData(entity.GetID());
        }
        
        template<typename T>
        const T& GetComponent(Entity entity) const {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            if (!IsEntityValid(entity)) {
                throw std::runtime_error("Entity is not valid");
            }
            
            return GetComponentArray<T>()->GetData(entity.GetID());
        }
        
        template<typename T>
        bool HasComponent(Entity entity) const {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            if (!IsEntityValid(entity)) {
                return false;
            }
            
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) == m_ComponentArrays.end()) {
                return false;
            }
            
            return GetComponentArray<T>()->HasData(entity.GetID());
        }
        
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            static_assert(std::is_base_of_v<System, T>, "T must inherit from System");
            
            SystemTypeID typeID = SystemTypeRegistry::GetTypeID<T>();
            
            if (m_Systems.find(typeID) != m_Systems.end()) {
                return std::static_pointer_cast<T>(m_Systems[typeID]);
            }
            
            auto system = std::make_shared<T>();
            m_Systems[typeID] = system;
            m_SystemsVector.push_back(system);
            
            std::sort(m_SystemsVector.begin(), m_SystemsVector.end(),
                [](const std::shared_ptr<System>& a, const std::shared_ptr<System>& b) {
                    return a->GetPriority() < b->GetPriority();
                });
            
            system->Init();
            return system;
        }
        
        template<typename T>
        void SetSystemSignature(const Signature& signature) {
            static_assert(std::is_base_of_v<System, T>, "T must inherit from System");
            
            SystemTypeID typeID = SystemTypeRegistry::GetTypeID<T>();
            
            if (m_Systems.find(typeID) == m_Systems.end()) {
                throw std::runtime_error("System not registered");
            }
            
            m_Systems[typeID]->SetSignature(signature);
            
            for (const auto& pair : m_Signatures) {
                EntityID entityID = pair.first;
                const Signature& entitySignature = pair.second;
                
                Entity entity(entityID);
                
                if (SignatureMatches(entitySignature, signature)) {
                    m_Systems[typeID]->AddEntity(entity);
                } else {
                    m_Systems[typeID]->RemoveEntity(entity);
                }
            }
        }
        
        template<typename T>
        std::shared_ptr<T> GetSystem() {
            static_assert(std::is_base_of_v<System, T>, "T must inherit from System");
            
            SystemTypeID typeID = SystemTypeRegistry::GetTypeID<T>();
            
            if (m_Systems.find(typeID) == m_Systems.end()) {
                return nullptr;
            }
            
            return std::static_pointer_cast<T>(m_Systems[typeID]);
        }
        
        void ecs_flush();
        void Update(float deltaTime);
        void Render();
        void Shutdown();
        
        size_t GetEntityCount() const { return m_LivingEntityCount; }
        size_t GetSystemCount() const { return m_Systems.size(); }
        size_t GetPendingOperationCount() const { return m_PendingOperations.size(); }
        
    private:
        std::queue<EntityID> m_AvailableEntities;
        std::unordered_map<EntityID, Signature> m_Signatures;
        EntityID m_NextEntityID;
        size_t m_LivingEntityCount;
        
        std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentArray>> m_ComponentArrays;
        
        std::unordered_map<SystemTypeID, std::shared_ptr<System>> m_Systems;
        std::vector<std::shared_ptr<System>> m_SystemsVector;
        
        std::vector<PendingOperation> m_PendingOperations;
        
        template<typename T>
        ComponentArray<T>* GetComponentArray() {
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) == m_ComponentArrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            
            return static_cast<ComponentArray<T>*>(m_ComponentArrays[typeID].get());
        }
        
        template<typename T>
        const ComponentArray<T>* GetComponentArray() const {
            ComponentTypeID typeID = ComponentTypeRegistry::GetTypeID<T>();
            
            if (m_ComponentArrays.find(typeID) == m_ComponentArrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            
            return static_cast<const ComponentArray<T>*>(m_ComponentArrays.at(typeID).get());
        }
        
        void UpdateEntitySystems(Entity entity);
        bool SignatureMatches(const Signature& entitySignature, const Signature& systemSignature) const;
    };
    
}
