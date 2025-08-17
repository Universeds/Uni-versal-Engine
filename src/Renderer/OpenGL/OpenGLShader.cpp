#include "OpenGLShader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace UniversalEngine {
    static GLenum ShaderTypeFromString(const std::string& type) {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
        
        return 0;
    }
    
    OpenGLShader::OpenGLShader(const std::string& filepath)
        : m_FilePath(filepath) {
        std::string source = ReadFile(filepath);
        auto shaderSources = PreProcess(source);
        
        size_t lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        size_t lastDot = filepath.rfind('.');
        size_t count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
        
        CompileOrGetVulkanBinaries(shaderSources);
        CompileOrGetOpenGLBinaries();
        CreateProgram();
    }
    
    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name) {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER] = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        
        CompileOrGetVulkanBinaries(sources);
        CompileOrGetOpenGLBinaries();
        CreateProgram();
    }
    
    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }
    
    std::string OpenGLShader::ReadFile(const std::string& filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in) {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1) {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
        }
        
        return result;
    }
    
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source) {
        std::unordered_map<GLenum, std::string> shaderSources;
        
        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = source.find_first_of("\r\n", pos);
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            
            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            pos = source.find(typeToken, nextLinePos);
            
            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }
        
        return shaderSources;
    }
    
    void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources) {
        m_OpenGLSourceCode = shaderSources;
    }
    
    void OpenGLShader::CompileOrGetOpenGLBinaries() {
        
    }
    
    void OpenGLShader::CreateProgram() {
        GLuint program = glCreateProgram();
        std::vector<GLenum> glShaderIDs;
        glShaderIDs.reserve(m_OpenGLSourceCode.size());
        
        for (auto& kv : m_OpenGLSourceCode) {
            GLenum type = kv.first;
            const std::string& source = kv.second;
            
            GLuint shader = glCreateShader(type);
            
            const GLchar* sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);
            
            glCompileShader(shader);
            
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
                
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
                
                glDeleteShader(shader);
                
                break;
            }
            
            glAttachShader(program, shader);
            glShaderIDs.push_back(shader);
        }
        
        m_RendererID = program;
        
        glLinkProgram(program);
        
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            
            glDeleteProgram(program);
            
            for (auto id : glShaderIDs)
                glDeleteShader(id);
            
            return;
        }
        
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }
    }
    
    void OpenGLShader::Bind() const {
        glUseProgram(m_RendererID);
    }
    
    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }
    
    void OpenGLShader::SetInt(const std::string& name, int value) {
        UploadUniformInt(name, value);
    }
    
    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) {
        UploadUniformIntArray(name, values, count);
    }
    
    void OpenGLShader::SetFloat(const std::string& name, float value) {
        UploadUniformFloat(name, value);
    }
    
    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) {
        UploadUniformFloat2(name, value);
    }
    
    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
        UploadUniformFloat3(name, value);
    }
    
    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
        UploadUniformFloat4(name, value);
    }
    
    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        UploadUniformMat4(name, value);
    }
    
    void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }
    
    void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }
    
    void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }
    
    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }
    
    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }
    
    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    
    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData) {
        
    }
}
