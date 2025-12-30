#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"

struct Staircase {
    Staircase() = default;
    Staircase(uint64_t id, StaircaseCreateInfo& createInfo, SpawnOffset& spawnOffset);
    Staircase(const Staircase&) = delete;
    Staircase& operator=(const Staircase&) = delete;
    Staircase(Staircase&&) noexcept = default;
    Staircase& operator=(Staircase&&) noexcept = default;
    ~Staircase() = default;

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void Update(float deltaTime);
    void RenderDebug();
    void CleanUp();

    //MeshNodes& GetMeshNodes()                           { return m_meshNodes; }
    const StaircaseCreateInfo& GetCreateInfo() const    { return m_createInfo; }
    const uint64_t GetObjectId() const                  { return m_objectId; }
    const glm::vec3& GetPosition() const                { return m_position; }
    const glm::vec3& GetRotation() const                { return m_rotation; }
    const std::vector<RenderItem>& GetRenderItems()     { return m_renderItems; }

private:
    void RecomputeModelMatrix();

    StaircaseCreateInfo m_createInfo;
    std::vector<MeshNodes> m_meshNodesList;
    std::vector<RenderItem> m_renderItems;
    uint64_t m_objectId = 0;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    glm::vec3 m_localForward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_worldForward = glm::vec3(0.0f);
};