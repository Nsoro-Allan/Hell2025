#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"

struct HousePlane {
    HousePlane() = default;
    HousePlane(uint64_t id, const HousePlaneCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    HousePlane(const HousePlane&) = delete;
    HousePlane& operator=(const HousePlane&) = delete;
    HousePlane(HousePlane&&) noexcept = default;
    HousePlane& operator=(HousePlane&&) noexcept = default;
    ~HousePlane() = default;

    void UpdateVertexDataFromCreateInfo();
    void UpdateWorldSpaceCenter(glm::vec3 worldSpaceCenter);
    void SetMaterial(const std::string& materialName);
    void SetMeshIndex(uint32_t index);
    void SetTextureScale(float value);
    void SetTextureOffsetU(float value);
    void SetTextureOffsetV(float value);
    void CleanUp();
    void SubmitRenderItem();
    void DrawEdges(glm::vec4 color);
    void DrawVertices(glm::vec4 color);

    const glm::vec3& GetWorldSpaceCenter() const    { return m_worldSpaceCenter; }
    const std::string& GetEditorName() const        { return m_createInfo.editorName; }
    const uint64_t GetObjectId() const              { return m_objectId; }
    const uint64_t GetParentDoorId() const          { return m_createInfo.parentDoorId; }
    const uint32_t GetMeshIndex() const             { return m_meshIndex; }
    Material* GetMaterial()                         { return m_material; };
    std::vector<Vertex>& GetVertices()              { return m_vertices; }
    std::vector<uint32_t>& GetIndices()             { return m_indices; }
    std::vector<glm::vec2>& GetNavMeshPoly()        { return m_navMeshPoly; }
    HousePlaneCreateInfo& GetCreateInfo()           { return m_createInfo; }
    HousePlaneType GetType() const                  { return m_createInfo.type; }

private:
    uint64_t m_objectId = 0;
    uint64_t m_parentDoorId = 0;
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
    std::vector<glm::vec2> m_navMeshPoly;
    HousePlaneCreateInfo m_createInfo;
    
    void CreatePhysicsObject();
};