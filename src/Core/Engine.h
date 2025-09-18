#pragma once
#include <memory>
#include <chrono>
#include "../Renderer/OpenGL/OpenGLContext.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace UniversalEngine {
    class Engine {
    public:
        Engine();
        virtual ~Engine();
        
        void Run();
        
        static Engine& Get() { return *s_Instance; }
        
    private:
        bool Init();
        void Shutdown();
        void Update();
        void Render();
        
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        
        // need to make a seperate component and system for this later
        void SetupCube();
        
    private:
        static Engine* s_Instance;
        
        GLFWwindow* m_Window;
        std::unique_ptr<OpenGLContext> m_Context;
        bool m_Running = true;
        
        uint32_t m_WindowWidth = 1280;
        uint32_t m_WindowHeight = 720;
        
        std::shared_ptr<VertexArray> m_CubeVertexArray;
        std::shared_ptr<Shader> m_CubeShader;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
        float m_Time = 0.0f;
        std::chrono::steady_clock::time_point lastDelta = std::chrono::steady_clock::now();
        float deltaTime = 0.0f;
    };
    
    Engine* CreateApplication();
}
