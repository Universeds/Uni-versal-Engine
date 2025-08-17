#include "OpenGLContext.h"
#include <iostream>

namespace UniversalEngine {
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle) {
    }
    
    void OpenGLContext::Init() {
        glfwMakeContextCurrent(m_WindowHandle);
        
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            return;
        }
        
        std::cout << "OpenGL Info:" << std::endl;
        std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glEnable(GL_DEPTH_TEST);
    }
    
    void OpenGLContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }
}
