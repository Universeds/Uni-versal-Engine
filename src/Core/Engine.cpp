#include "Engine.h"
#include "../Renderer/Renderer.h"
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
        
        // Setup cube rendering
        SetupCube();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsLight();
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 450");
        
        return true;
    }
    
    void Engine::Shutdown() {
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
    
    void Engine::SetupCube() {
        // Vertex and indices setup by chatgpt
        float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-left (red)
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right (green)
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-right (blue)
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Top-left (yellow)
            
            // Back face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-left (magenta)
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Bottom-right (cyan)
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, // Top-right (white)
            -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f  // Top-left (gray)
        };
        
        // Cube indices
        uint32_t indices[] = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            7, 3, 0, 0, 4, 7,
            // Right face
            1, 5, 6, 6, 2, 1,
            // Top face
            3, 2, 6, 6, 7, 3,
            // Bottom face
            0, 1, 5, 5, 4, 0
        };
        
        m_CubeVertexArray = VertexArray::Create();
        
        auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Color" }
        });
        m_CubeVertexArray->AddVertexBuffer(vertexBuffer);
        
        auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_CubeVertexArray->SetIndexBuffer(indexBuffer);
        
        std::string vertexShaderSource = R"(
            #version 450 core
            
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec3 a_Color;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
            
            out vec3 v_Color;
            
            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
            }
        )";
        
        std::string fragmentShaderSource = R"(
            #version 450 core
            
            layout(location = 0) out vec4 color;
            
            in vec3 v_Color;
            
            void main() {
                color = vec4(v_Color, 1.0);
            }
        )";
        
        m_CubeShader = Shader::Create("CubeShader", vertexShaderSource, fragmentShaderSource);
        
        m_ViewMatrix = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        
        m_ProjectionMatrix = glm::perspective(
            glm::radians(45.0f), 
            (float)m_WindowWidth / (float)m_WindowHeight, 
            0.1f, 
            100.0f
        );
    }
    
    void Engine::Update() {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        
        deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastDelta).count() / 1000000.0f;

        m_Time += deltaTime;
        lastDelta = now;

        //ImGUI 
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Hello, world!");

            ImGui::Checkbox("Application is WORKING MAN", &m_Running);

            ImGui::SliderFloat("float", &m_Time, 0.0f, 1.0f);
            ImGui::DragFloat3("Projection Matrix", (float*)&m_ProjectionMatrix);

            if (ImGui::Button("Button"))
                std::cout << "Button Pressed" << std::endl;
            ImGui::SameLine();
            ImGui::Text("counter = %d", (int)m_Time);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
    }
    
    void Engine::Render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Create rotating transform matrix
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), m_Time, glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 viewProjection = m_ProjectionMatrix * m_ViewMatrix;
        
        m_CubeShader->Bind();
        m_CubeShader->SetMat4("u_ViewProjection", viewProjection);
        m_CubeShader->SetMat4("u_Transform", transform);
        
        // Render the cube
        Renderer::Submit(m_CubeVertexArray);
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        engine->m_WindowWidth = width;
        engine->m_WindowHeight = height;
        Renderer::OnWindowResize(width, height);
    }
}
