#pragma once
#include "../ECS/System.h"
#include "../ECS/World.h"
#include "../Components/Transform2D.h"
#include "../Components/MeshRenderer2D.h"
#include "../../Renderer/Shader.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Buffer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace UniversalEngine {
    
    class RenderSystem2D : public System {
    public:
        RenderSystem2D() = default;
        ~RenderSystem2D() = default;
        
        void Init() override {
            SetupShader();
            SetupQuadVAO();
        }
        
        void Update(float deltaTime) override {
        }
        
        void Render(World& world) {
            if (!m_Shader || !m_QuadVAO) {
                return;
            }
            
            m_Shader->Bind();
            
            glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -7.5f, 7.5f, -1.0f, 1.0f);
            m_Shader->SetMat4("u_Projection", projection);
            
            for (auto entity : m_Entities) {
                if (!world.HasComponent<Transform2D>(entity) || 
                    !world.HasComponent<MeshRenderer2D>(entity)) {
                    continue;
                }
                
                auto& transform = world.GetComponent<Transform2D>(entity);
                auto& meshRenderer = world.GetComponent<MeshRenderer2D>(entity);
                
                if (!meshRenderer.visible) {
                    continue;
                }
                
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(transform.position, 0.0f));
                model = glm::rotate(model, glm::radians((float)transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(transform.scale * meshRenderer.size, 1.0f));
                
                m_Shader->SetMat4("u_Model", model);
                m_Shader->SetFloat4("u_Color", meshRenderer.color);
                
                m_QuadVAO->Bind();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }
        
        void Shutdown() override {
            m_Shader.reset();
            m_QuadVAO.reset();
        }
        
    private:
        void SetupShader() {
            std::string vertexShaderSource = R"(
                #version 450 core
                
                layout(location = 0) in vec2 a_Position;
                
                uniform mat4 u_Projection;
                uniform mat4 u_Model;
                
                void main() {
                    gl_Position = u_Projection * u_Model * vec4(a_Position, 0.0, 1.0);
                }
            )";
            
            std::string fragmentShaderSource = R"(
                #version 450 core
                
                layout(location = 0) out vec4 color;
                
                uniform vec4 u_Color;
                
                void main() {
                    color = u_Color;
                }
            )";
            
            m_Shader = Shader::Create("2DShader", vertexShaderSource, fragmentShaderSource);
        }
        
        void SetupQuadVAO() {
            float vertices[] = {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.5f,  0.5f,
                -0.5f,  0.5f
            };
            
            uint32_t indices[] = {
                0, 1, 2,
                2, 3, 0
            };
            
            m_QuadVAO = VertexArray::Create();
            
            auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
            vertexBuffer->SetLayout({
                { ShaderDataType::Float2, "a_Position" }
            });
            m_QuadVAO->AddVertexBuffer(vertexBuffer);
            
            auto indexBuffer = IndexBuffer::Create(indices, 6);
            m_QuadVAO->SetIndexBuffer(indexBuffer);
        }
        
    private:
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_QuadVAO;
    };
    
}
