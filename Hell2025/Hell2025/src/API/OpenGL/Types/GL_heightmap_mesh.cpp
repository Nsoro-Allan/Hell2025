#pragma once
#include "GL_heightmap_mesh.h"
#include "HellLogging.h"
#include "HellGlm.h"
#include <glad/glad.h>

void OpenGLHeightMapMesh::Create() {
    if (m_vao != 0) {
        CleanUp();
    }
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(0);
}

void OpenGLHeightMapMesh::AllocateMemory(int chunkCount) {
    if (m_vao == 0) Create();

    // cells per side of a chunk (e.g. 32) -> 33x33 verts
    constexpr int CHUNK_CELLS = 32;
    const int chunkWidth = CHUNK_CELLS + 1; // 33
    const int chunkDepth = CHUNK_CELLS + 1; // 33

    const int vertsPerChunk = chunkWidth * chunkDepth;                 // 1089
    const int indicesPerChunk = (CHUNK_CELLS * CHUNK_CELLS) * 6;         // 6144

    const int localVertexBytes = vertsPerChunk * sizeof(Vertex);
    const int localIndexBytes = indicesPerChunk * sizeof(uint32_t);

    const int totalVertexBytes = localVertexBytes * chunkCount;          // 6,133,248 (if Vertex=44B)
    const int totalIndexBytes = localIndexBytes * chunkCount;          // 3,145,728

    if (m_totalVertexBufferSize < totalVertexBytes || m_totalIndexBufferSize < totalIndexBytes) {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, totalVertexBytes, nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexBytes, nullptr, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        m_totalVertexBufferSize = totalVertexBytes;
        m_totalIndexBufferSize = totalIndexBytes;

        Logging::Debug() << "Allocated heightmap memory: " << m_totalVertexBufferSize << " verts-bytes " << m_totalIndexBufferSize << " index-bytes";
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OpenGLHeightMapMesh::CleanUp() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
}

int OpenGLHeightMapMesh::GetVAO() {
    return m_vao;
}

int OpenGLHeightMapMesh::GetVBO() {
    return m_vbo;
}

int OpenGLHeightMapMesh::GetEBO() {
    return m_ebo;
}
