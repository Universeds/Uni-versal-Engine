#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace UniversalEngine {
    class Cube {
    public:
        Cube();
        ~Cube() = default;
        
        void Render(const std::shared_ptr<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
        
        std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
        
    private:
        void SetupMesh();
        
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}
