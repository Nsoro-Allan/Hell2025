#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"

struct Fireplace {
    Fireplace() = default;
    Fireplace(uint64_t id, const FireplaceCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    Fireplace(const Fireplace&) = delete;
    Fireplace& operator=(const Fireplace&) = delete;
    Fireplace(Fireplace&&) noexcept = default;
    Fireplace& operator=(Fireplace&&) noexcept = default;
    ~Fireplace() = default;

    void Update(float deltaTime);
    void CleanUp();

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);

    uint64_t GetObjectId()                                      { return m_id; }
    AABB GetWallsAABB()                                         { return m_wallsAabb; }
	MeshNodes& GetMeshNodes()                                   { return m_meshNodes; }
	const std::vector<RenderItem>& GetRenderItems() const       { return m_meshNodes.GetRenderItems(); }
    const std::vector<RenderItem>& GetRenderItemsGlass() const  { return m_meshNodes.GetRenderItemsGlass(); }
    const glm::vec3& GetPosition()                              { return m_transform.position; }
    const glm::vec3& GetRotation()                              { return m_transform.rotation; }
    const FireplaceCreateInfo& GetCreateInfo() const            { return m_createInfo; }
    const glm::mat4& GetWorldMatrix() const                     { return m_worldMatrix; }
    float GetWallDepth() const                                  { return m_wallDepth; }
    float GetWallWidth() const                                  { return m_wallWidth; }
    const glm::vec3 GetWorldForward() const                     { return m_worldForward; }
    const glm::vec3 GetWorldRight() const                       { return m_worldRight; }

private:
    void UpdateWorldMatrix();
    uint64_t m_id = 0;
    FireplaceCreateInfo m_createInfo;
    MeshNodes m_meshNodes;
    Transform m_transform;
    glm::mat4 m_worldMatrix = glm::mat4(1.0f);
    glm::vec3 m_worldForward = glm::vec3(0.0f);
    glm::vec3 m_worldRight = glm::vec3(0.0f);
    AABB m_wallsAabb;
    float m_wallDepth = 0.0f;
    float m_wallWidth = 0.0f;
};