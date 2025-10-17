#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"

struct Plane {
    void Init(PlaneCreateInfo createInfo);
    void UpdateVertexDataFromCreateInfo();
    void UpdateWorldSpaceCenter(glm::vec3 worldSpaceCenter);
    void SetMaterial(const std::string& materialName);
    void SetMeshIndex(uint32_t index);
    void CleanUp();
    void SubmitRenderItem();
    void DrawEdges(glm::vec4 color);
    void DrawVertices(glm::vec4 color);

    const glm::vec3& GetWorldSpaceCenter() const    { return m_worldSpaceCenter; }
    const uint64_t GetObjectId() const              { return m_objectId; }
    const uint32_t GetMeshIndex() const             { return m_meshIndex; }
    Material* GetMaterial()                         { return m_material; };
    std::vector<Vertex>& GetVertices()              { return m_vertices; }
    std::vector<uint32_t>& GetIndices()             { return m_indices; }
    PlaneCreateInfo& GetCreateInfo()     { return m_createInfo; }

private:
    uint64_t m_objectId = 0;
    uint64_t m_physicsId = 0;
    uint32_t m_meshIndex = 0;
    Material* m_material = nullptr;
    glm::vec3 m_p0 = glm::vec3(0.0f);
    glm::vec3 m_p1 = glm::vec3(0.0f);
    glm::vec3 m_p2 = glm::vec3(0.0f);
    glm::vec3 m_p3 = glm::vec3(0.0f);
    glm::vec3 m_worldSpaceCenter = glm::vec3(0.0f);
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    PlaneCreateInfo m_createInfo;
    
    void CreatePhysicsObject();
};