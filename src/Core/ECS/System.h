#pragma once
#include <cstdint>
#include <set>
#include <memory>
#include <typeinfo>
#include "Entity.h"
#include "Component.h"

namespace UniversalEngine {
    
    using SystemTypeID = std::uint32_t;
    
    class System {
    public:
        System() = default;
        virtual ~System() = default;
        
        System(const System&) = delete;
        System& operator=(const System&) = delete;
        
        System(System&&) = default;
        System& operator=(System&&) = default;
        
        virtual void Init() {}
        virtual void Update(float deltaTime) {}
        virtual void Render() {}
        virtual void Shutdown() {}
        
        virtual void OnEntityAdded(Entity entity) {}
        virtual void OnEntityRemoved(Entity entity) {}
        
        void SetSignature(const std::set<ComponentTypeID>& signature) {
            m_Signature = signature;
        }
        
        const std::set<ComponentTypeID>& GetSignature() const {
            return m_Signature;
        }
        
        void AddEntity(Entity entity) {
            m_Entities.insert(entity);
            OnEntityAdded(entity);
        }
        
        void RemoveEntity(Entity entity) {
            m_Entities.erase(entity);
            OnEntityRemoved(entity);
        }
        
        const std::set<Entity>& GetEntities() const {
            return m_Entities;
        }
        
        size_t GetEntityCount() const {
            return m_Entities.size();
        }
        
        bool HasEntity(Entity entity) const {
            return m_Entities.find(entity) != m_Entities.end();
        }
        
        void SetPriority(int priority) { m_Priority = priority; }
        int GetPriority() const { return m_Priority; }
        
        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }
        
    protected:
        std::set<Entity> m_Entities;
        
        std::set<ComponentTypeID> m_Signature;
        
        int m_Priority = 0;
        
        bool m_Enabled = true;
    };
    
    class SystemTypeRegistry {
    public:
        template<typename T>
        static SystemTypeID GetTypeID() {
            static_assert(std::is_base_of_v<System, T>, "T must inherit from System");
            
            static SystemTypeID typeID = s_NextTypeID++;
            return typeID;
        }
        
        static SystemTypeID GetNextTypeID() { return s_NextTypeID; }
        
    private:
        static SystemTypeID s_NextTypeID;
    };
    
    class UpdateSystem : public System {
    public:
        virtual void Update(float deltaTime) override = 0;
    };
    
    class RenderSystem : public System {
    public:
        virtual void Render() override = 0;
    };
    
    class EventSystem : public System {
    public:
        virtual void HandleEvent(const void* event) {}
    };
    
    class PhysicsSystem : public UpdateSystem {
    public:
        virtual void FixedUpdate(float fixedDeltaTime) {}
    };
    
}
