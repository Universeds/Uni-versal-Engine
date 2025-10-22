#include "Engine.h"
#include "../Renderer/Renderer.h"
#include "Scenes/SimpleScene2D.h"
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace UniversalEngine {
    Engine* Engine::s_Instance = nullptr;
    
    Engine::Engine() {
        s_Instance = this;
    }
    
    Engine::~Engine() {
        Shutdown();
    }
    
    bool Engine::Init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "Uni-versal Engine", nullptr, nullptr);
        if (!m_Window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        m_Context = std::make_unique<OpenGLContext>(m_Window);
        m_Context->Init();
        
        glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
        glfwSetWindowUserPointer(m_Window, this);
        
        Renderer::Init();
        
        m_World = std::make_unique<World>();
        SetupScene();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        io.FontGlobalScale = 1.5f;
        
        ImGui::StyleColorsLight();
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        
        return true;
    }
    
    void Engine::Shutdown() {
        if (m_RenderSystem) {
            m_RenderSystem->Shutdown();
        }
        
        m_World.reset();
        
        Renderer::Shutdown();
        
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    }
    
    void Engine::Run() {
        if (!Init()) {
            return;
        }
        
        while (m_Running && !glfwWindowShouldClose(m_Window)) {
            glfwPollEvents();
            
            Update();
            Render();
            
            m_Context->SwapBuffers();
        }
    }
    
    void Engine::SetupScene() {
        SimpleScene2D::CreateScene(*m_World);
        
        m_RenderSystem = m_World->RegisterSystem<RenderSystem2D>();
        m_RenderSystem->SetViewportSize(m_WindowWidth, m_WindowHeight);
        
        Signature renderSignature;
        renderSignature.insert(ComponentTypeRegistry::GetTypeID<Transform2D>());
        renderSignature.insert(ComponentTypeRegistry::GetTypeID<MeshRenderer2D>());
        m_World->SetSystemSignature<RenderSystem2D>(renderSignature);
        
        m_PhysicsSystem = m_World->RegisterSystem<Physics2DSystem>();
        m_PhysicsSystem->SetWorld(m_World.get());
        
        Signature physicsSignature;
        physicsSignature.insert(ComponentTypeRegistry::GetTypeID<Transform2D>());
        physicsSignature.insert(ComponentTypeRegistry::GetTypeID<BoxCollider2D>());
        m_World->SetSystemSignature<Physics2DSystem>(physicsSignature);
        
        m_MouseInteractionSystem = m_World->RegisterSystem<MouseInteractionSystem>();
        m_MouseInteractionSystem->SetWorld(m_World.get());
        m_MouseInteractionSystem->SetWindow(m_Window);
        m_MouseInteractionSystem->SetViewportSize(m_WindowWidth, m_WindowHeight);
        
        Signature mouseSignature;
        mouseSignature.insert(ComponentTypeRegistry::GetTypeID<Transform2D>());
        mouseSignature.insert(ComponentTypeRegistry::GetTypeID<BoxCollider2D>());
        mouseSignature.insert(ComponentTypeRegistry::GetTypeID<Rigidbody2D>());
        m_World->SetSystemSignature<MouseInteractionSystem>(mouseSignature);
        
        m_World->ecs_flush();
    }
    
    void Engine::Update() {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        
        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastDelta).count() / 1000000.0f;

        m_Time += deltaTime;
        lastDelta = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Uni-versal Engine");

            ImGui::Checkbox("Application Running", &m_Running);

            ImGui::Text("Time: %.2f", m_Time);
            ImGui::Text("Entity Count: %zu", m_World->GetEntityCount());

            if (ImGui::Button("Button"))
                std::cout << "Button Pressed" << std::endl;

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();

            
            ImGui::Begin("Entity Inspector");
            
            for (EntityID entityID = 1; entityID < m_World->GetEntityCount() + 1; ++entityID) {
                Entity entity(entityID);
                
                if (!m_World->IsEntityValid(entity)) {
                    continue;
                }
                
                if (ImGui::TreeNode((void*)(intptr_t)entityID, "Entity %u", entityID)) {
                    if (m_World->HasComponent<Transform2D>(entity)) {
                        auto& transform = m_World->GetComponent<Transform2D>(entity);
                        if (ImGui::TreeNode("Transform2D")) {
                            ImGui::DragFloat2("Position", &transform.position.x, 0.1f);
                            ImGui::SliderFloat("Rotation", &transform.rotation, -180.0f, 180.0f);
                            ImGui::DragFloat2("Scale", &transform.scale.x, 0.01f, 0.01f, 10.0f);
                            ImGui::TreePop();
                        }
                    }
                    
                    if (m_World->HasComponent<Rigidbody2D>(entity)) {
                        auto& rb = m_World->GetComponent<Rigidbody2D>(entity);
                        if (ImGui::TreeNode("Rigidbody2D")) {
                            ImGui::DragFloat2("Velocity", &rb.velocity.x, 0.1f);
                            ImGui::SliderFloat("Angular Velocity", &rb.angularVelocity, -10.0f, 10.0f);
                            ImGui::SliderFloat("Mass", &rb.mass, 0.1f, 10.0f);
                            ImGui::SliderFloat("Restitution", &rb.restitution, 0.0f, 1.0f);
                            ImGui::SliderFloat("Friction", &rb.friction, 0.0f, 1.0f);
                            ImGui::SliderFloat("Gravity Scale", &rb.gravityScale, 0.0f, 2.0f);
                            ImGui::SliderFloat("Drag", &rb.drag, 0.0f, 0.5f);
                            ImGui::SliderFloat("Angular Drag", &rb.angularDrag, 0.0f, 0.5f);
                            ImGui::Checkbox("Use Gravity", &rb.useGravity);
                            ImGui::TreePop();
                        }
                    }
                    
                    if (m_World->HasComponent<MeshRenderer2D>(entity)) {
                        auto& mesh = m_World->GetComponent<MeshRenderer2D>(entity);
                        if (ImGui::TreeNode("MeshRenderer2D")) {
                            ImGui::DragFloat2("Size", &mesh.size.x, 0.1f, 0.1f, 10.0f);
                            ImGui::ColorEdit4("Color", &mesh.color.x);
                            ImGui::Checkbox("Visible", &mesh.visible);
                            ImGui::TreePop();
                        }
                    }
                    
                    if (m_World->HasComponent<BoxCollider2D>(entity)) {
                        auto& collider = m_World->GetComponent<BoxCollider2D>(entity);
                        if (ImGui::TreeNode("BoxCollider2D")) {
                            ImGui::DragFloat2("Size", &collider.size.x, 0.1f, 0.1f, 10.0f);
                            ImGui::DragFloat2("Offset", &collider.offset.x, 0.1f);
                            ImGui::Checkbox("Is Trigger", &collider.isTrigger);
                            ImGui::Checkbox("Is Static", &collider.isStatic);
                            ImGui::TreePop();
                        }
                    }
                    
                    ImGui::TreePop();
                }
            }
            
            ImGui::End();
        }
        
        m_World->Update(deltaTime);
    }
    
    void Engine::Render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (m_RenderSystem) {
            m_RenderSystem->Render(*m_World);
        }
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->m_WindowWidth = width;
        engine->m_WindowHeight = height;
        Renderer::OnWindowResize(width, height);
        
        // Update render system viewport size to maintain correct aspect ratio
        if (engine->m_RenderSystem) {
            engine->m_RenderSystem->SetViewportSize(width, height);
        }
        
        // Update mouse interaction system viewport size for correct coordinate transformation
        if (engine->m_MouseInteractionSystem) {
            engine->m_MouseInteractionSystem->SetViewportSize(width, height);
        }
    }
}
