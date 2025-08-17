#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace UniversalEngine {
    class OpenGLContext {
    public:
        OpenGLContext(GLFWwindow* windowHandle);
        
        void Init();
        void SwapBuffers();
        
    private:
        GLFWwindow* m_WindowHandle;
    };
}
