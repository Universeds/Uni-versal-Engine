#include "Engine.h"
#include "../Renderer/Renderer.h"
#include <iostream>

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
        
        return true;
    }
    
    void Engine::Shutdown() {
        Renderer::Shutdown();
        
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        
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
    
    void Engine::Update() {
        
    }
    
    void Engine::Render() {
        
    }
    
    void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->m_WindowWidth = width;
        engine->m_WindowHeight = height;
        Renderer::OnWindowResize(width, height);
    }
}
