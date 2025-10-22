#pragma once
#include <memory>
#include <chrono>
#include "../Renderer/OpenGL/OpenGLContext.h"
#include "ECS/World.h"
#include "Systems/RenderSystem2D.h"
#include "Systems/Physics2DSystem.h"
#include "Systems/MouseInteractionSystem.h"

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
        
        void SetupScene();
        
    private:
        static Engine* s_Instance;
        
        GLFWwindow* m_Window;
        std::unique_ptr<OpenGLContext> m_Context;
        bool m_Running = true;
        
        uint32_t m_WindowWidth = 1280;
        uint32_t m_WindowHeight = 720;
        
        float m_Time = 0.0f;
        std::chrono::steady_clock::time_point lastDelta = std::chrono::steady_clock::now();
        float deltaTime = 0.0f;
        
        std::unique_ptr<World> m_World;
        std::shared_ptr<RenderSystem2D> m_RenderSystem;
        std::shared_ptr<Physics2DSystem> m_PhysicsSystem;
        std::shared_ptr<MouseInteractionSystem> m_MouseInteractionSystem;
        
        Entity m_SceneEntity;
    };
    
    Engine* CreateApplication();
}
