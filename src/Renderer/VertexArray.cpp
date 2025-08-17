#include "VertexArray.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "Renderer.h"

namespace UniversalEngine {
    std::shared_ptr<VertexArray> VertexArray::Create() {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
        }
        
        return nullptr;
    }
}
