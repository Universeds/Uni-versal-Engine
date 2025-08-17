#include "Shader.h"
#include "OpenGL/OpenGLShader.h"
#include "Renderer.h"
#include <memory>

namespace UniversalEngine {
    std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
        }
        
        return nullptr;
    }
    
    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
        switch (Renderer::GetAPI()) {
            case RendererAPI::API::None:    return nullptr;
            case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        
        return nullptr;
    }
    
    void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
        m_Shaders[name] = shader;
    }
    
    void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
        auto& name = shader->GetName();
        Add(name, shader);
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }
    
    std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
        return m_Shaders[name];
    }
    
    bool ShaderLibrary::Exists(const std::string& name) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }
}
