#pragma once
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include "HellTypes.h"

struct OpenGLMesh {
    OpenGLMesh() = default;
    void CleanUp();

    size_t GetVertexCount() const { return static_cast<size_t>(m_vertexCount); }
    unsigned int GetVAO() const   { return m_vao; }

    template<typename TVertex>
    void SetupVertexAttributes();

    template<typename TVertex>

    void UpdateVertexData(const std::vector<TVertex>& vertices) {
        if (m_vao == 0) {
            glGenVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);
        }

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        GLsizeiptr bufferSize = (GLsizeiptr)(vertices.size() * sizeof(TVertex));
        if (bufferSize > m_allocatedBufferSize) {
            glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
            m_allocatedBufferSize = bufferSize;
        }

        if (!vertices.empty()) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, vertices.data());
        }

        SetupVertexAttributes<TVertex>();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_vertexCount = (GLsizeiptr)vertices.size();
    }

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLsizeiptr m_allocatedBufferSize = 0;
    GLsizeiptr m_vertexCount = 0;
};

// DebugVertex3D layout
template<>
inline void OpenGLMesh::SetupVertexAttributes<DebugVertex3D>() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, color));

    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 2, GL_INT, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, pixelOffset));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, depthEnabled));

    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 1, GL_INT, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, exclusiveViewportIndex));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 1, GL_INT, sizeof(DebugVertex3D), (void*)offsetof(DebugVertex3D, ignoredViewportIndex));
}

// DebugVertex2D layout
template<>
inline void OpenGLMesh::SetupVertexAttributes<DebugVertex2D>() {
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 2, GL_INT, sizeof(DebugVertex2D), (void*)offsetof(DebugVertex2D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex2D), (void*)offsetof(DebugVertex2D, color));
}
