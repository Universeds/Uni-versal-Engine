#pragma once
#include <cstdint>
#include <typeinfo>
#include <unordered_map>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include "Entity.h"

namespace UniversalEngine {
    
    using ComponentTypeID = std::uint32_t;
    
    class Component {
    public:
        Component() = default;
        virtual ~Component() = default;
        
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
        
        Component(Component&&) = default;
        Component& operator=(Component&&) = default;
    };
    
    class ComponentTypeRegistry {
    public:
        template<typename T>
        static ComponentTypeID GetTypeID() {
            static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
            
            static ComponentTypeID typeID = s_NextTypeID++;
            return typeID;
        }
        
        static ComponentTypeID GetNextTypeID() { return s_NextTypeID; }
        
    private:
        static ComponentTypeID s_NextTypeID;
    };
    
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityID entity) = 0;
        virtual size_t Size() const = 0;
    };
    
    template<typename T>
    class ComponentArray : public IComponentArray {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        
    public:
        void InsertData(EntityID entity, T component) {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
                size_t index = m_EntityToIndexMap[entity];
                m_ComponentArray[index] = std::move(component);
                return;
            }
            
            size_t newIndex = m_Size;
            m_EntityToIndexMap[entity] = newIndex;
            m_IndexToEntityMap[newIndex] = entity;
            
            if (newIndex >= m_ComponentArray.size()) {
                m_ComponentArray.resize(newIndex + 1);
            }
            
            m_ComponentArray[newIndex] = std::move(component);
            ++m_Size;
        }
        
        void RemoveData(EntityID entity) {
            if (m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end()) {
                return;
            }
            
            size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
            size_t indexOfLastElement = m_Size - 1;
            m_ComponentArray[indexOfRemovedEntity] = std::move(m_ComponentArray[indexOfLastElement]);
            
            EntityID entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
            m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;
            
            m_EntityToIndexMap.erase(entity);
            m_IndexToEntityMap.erase(indexOfLastElement);
            
            --m_Size;
        }
        
        T& GetData(EntityID entity) {
            if (m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end()) {
                throw std::runtime_error("Entity does not have this component");
            }
            
            return m_ComponentArray[m_EntityToIndexMap[entity]];
        }
        
        const T& GetData(EntityID entity) const {
            if (m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end()) {
                throw std::runtime_error("Entity does not have this component");
            }
            
            return m_ComponentArray[m_EntityToIndexMap.at(entity)];
        }
        
        bool HasData(EntityID entity) const {
            return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
        }
        
        void EntityDestroyed(EntityID entity) override {
            if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
                RemoveData(entity);
            }
        }
        
        size_t Size() const override {
            return m_Size;
        }
        
        T* begin() { return m_ComponentArray.data(); }
        T* end() { return m_ComponentArray.data() + m_Size; }
        const T* begin() const { return m_ComponentArray.data(); }
        const T* end() const { return m_ComponentArray.data() + m_Size; }
        
    private:
        std::vector<T> m_ComponentArray;
        
        std::unordered_map<EntityID, size_t> m_EntityToIndexMap;
        
        std::unordered_map<size_t, EntityID> m_IndexToEntityMap;
        
        size_t m_Size = 0;
    };
    
    class Entity;
    
}
