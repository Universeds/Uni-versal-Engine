#pragma once
#include "../ECS/System.h"
#include "../ECS/World.h"
#include "../Components/Transform2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/BoxCollider2D.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace UniversalEngine {
    
    class MouseInteractionSystem : public System {
    public:
        MouseInteractionSystem() = default;
        ~MouseInteractionSystem() = default;
        
        void Init() override {
        }
        
        void SetViewportSize(uint32_t width, uint32_t height) {
            m_ViewportWidth = width;
            m_ViewportHeight = height;
        }
        
        void Update(float deltaTime) override {
            if (!m_World || !m_Window) return;
            
            double mouseX, mouseY;
            glfwGetCursorPos(m_Window, &mouseX, &mouseY);
            
            int windowWidth, windowHeight;
            glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);
            
            glm::vec2 worldPos = ScreenToWorld(glm::vec2(mouseX, mouseY), windowWidth, windowHeight);
            
            int leftMouseButton = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
            
            if (leftMouseButton == GLFW_PRESS && !m_IsDragging) {
                m_GrabbedEntity = FindEntityAtPosition(worldPos);
                if (m_GrabbedEntity.GetID() != 0) {
                    m_IsDragging = true;
                    auto& transform = m_World->GetComponent<Transform2D>(m_GrabbedEntity);
                    auto& rigidbody = m_World->GetComponent<Rigidbody2D>(m_GrabbedEntity);
                    wasUsingGravity = rigidbody.useGravity;
                    m_DragOffset = transform.position - worldPos;
                }
            } else if (leftMouseButton == GLFW_RELEASE && m_IsDragging) {
                m_IsDragging = false;
                if (m_World->HasComponent<Transform2D>(m_GrabbedEntity) &&
                    m_World->HasComponent<Rigidbody2D>(m_GrabbedEntity)) {   
                    auto& rigidbody = m_World->GetComponent<Rigidbody2D>(m_GrabbedEntity);
                    rigidbody.useGravity = wasUsingGravity;
                }
                m_GrabbedEntity = Entity(0);
            }
            
            if (m_IsDragging && m_GrabbedEntity.GetID() != 0) {
                if (m_World->HasComponent<Transform2D>(m_GrabbedEntity) &&
                    m_World->HasComponent<Rigidbody2D>(m_GrabbedEntity)) {
                    
                    auto& transform = m_World->GetComponent<Transform2D>(m_GrabbedEntity);
                    auto& rigidbody = m_World->GetComponent<Rigidbody2D>(m_GrabbedEntity);
                    
                    glm::vec2 targetPos = worldPos + m_DragOffset;
                    glm::vec2 direction = targetPos - transform.position;
                    
                    float stiffness = 20.0f;
                    float damping = 0.8f;
                    
                    rigidbody.velocity = direction * stiffness - rigidbody.velocity * damping;
                    
                    rigidbody.useGravity = false;
                }
            }
        }
        
        void SetWorld(World* world) {
            m_World = world;
        }
        
        void SetWindow(GLFWwindow* window) {
            m_Window = window;
        }
        
    private:
        glm::vec2 ScreenToWorld(const glm::vec2& screenPos, int windowWidth, int windowHeight) {
            float normalizedX = (screenPos.x / windowWidth) * 2.0f - 1.0f;
            float normalizedY = 1.0f - (screenPos.y / windowHeight) * 2.0f;
            
            // Calculate aspect ratio and world dimensions to match the render system
            float aspectRatio = static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight);
            float orthoHeight = 10.0f;
            float orthoWidth = orthoHeight * aspectRatio;
            
            glm::vec2 worldPos;
            worldPos.x = normalizedX * orthoWidth;
            worldPos.y = normalizedY * orthoHeight;
            
            return worldPos;
        }
        
        Entity FindEntityAtPosition(const glm::vec2& worldPos) {
            for (auto entity : m_Entities) {
                if (!m_World->HasComponent<Transform2D>(entity) ||
                    !m_World->HasComponent<BoxCollider2D>(entity)) {
                    continue;
                }
                
                auto& transform = m_World->GetComponent<Transform2D>(entity);
                auto& collider = m_World->GetComponent<BoxCollider2D>(entity);
                
                glm::vec2 min = transform.position + collider.offset - collider.size * transform.scale * 0.5f;
                glm::vec2 max = transform.position + collider.offset + collider.size * transform.scale * 0.5f;
                
                if (worldPos.x >= min.x && worldPos.x <= max.x &&
                    worldPos.y >= min.y && worldPos.y <= max.y) {
                    if (m_World->HasComponent<Rigidbody2D>(entity)) {
                        return entity;
                    }
                }
            }
            
            return Entity(0);
        }
        
    private:
        World* m_World = nullptr;
        GLFWwindow* m_Window = nullptr;
        bool m_IsDragging = false;
        bool wasUsingGravity = false;
        Entity m_GrabbedEntity{0};
        glm::vec2 m_DragOffset{0.0f, 0.0f};
        uint32_t m_ViewportWidth = 1280;
        uint32_t m_ViewportHeight = 720;
    };
    
}
