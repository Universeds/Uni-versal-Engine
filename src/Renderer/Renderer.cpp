#include "Renderer.h"
#include "OpenGL/OpenGLRenderer.h"

namespace UniversalEngine {
    std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = nullptr;
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
    
    std::unique_ptr<RendererAPI> RendererAPI::Create() {
        switch (s_API) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLRendererAPI>();
        }
        
        return nullptr;
    }
    
    void Renderer::Init() {
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
    }
    
    void Renderer::Shutdown() {
        s_RendererAPI.reset();
    }
    
    void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
        s_RendererAPI->SetViewport(0, 0, width, height);
    }
    
    void Renderer::BeginScene() {
        
    }
    
    void Renderer::EndScene() {
        
    }
    
    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray) {
        vertexArray->Bind();
        s_RendererAPI->DrawIndexed(vertexArray);
    }
}
