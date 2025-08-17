#pragma once
#include <memory>
#include "../Renderer/OpenGL/OpenGLContext.h"

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
        
    private:
        static Engine* s_Instance;
        
        GLFWwindow* m_Window;
        std::unique_ptr<OpenGLContext> m_Context;
        bool m_Running = true;
        
        uint32_t m_WindowWidth = 1280;
        uint32_t m_WindowHeight = 720;
    };
    
    Engine* CreateApplication();
}
