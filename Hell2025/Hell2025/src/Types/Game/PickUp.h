#pragma once
#include "HellTypes.h"
#include "HellEnums.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"

struct PickUp {
    PickUp() = default;
    PickUp(uint64_t id, const PickUpCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    PickUp(const PickUp&) = delete;
    PickUp& operator=(const PickUp&) = delete;
    PickUp(PickUp&&) noexcept = default;
    PickUp& operator=(PickUp&&) noexcept = default;
    ~PickUp() = default;

    void SetPosition(const glm::vec3& position);
    void Update(float deltaTime);
    void CleanUp();

    const PickUpCreateInfo& GetCreateInfo() const       { return m_createInfo; }
    const PickUpType GetType() const                    { return m_createInfo.type; }

    const std::vector<RenderItem>& GetRenderItems()     { return m_meshNodes.GetRenderItems(); }
    const glm::vec3 GetPosition()                       { return m_modelMatrix[3]; }
    const glm::vec3& GetRotation()                      { return m_initialTransform.rotation; }
    const glm::mat4& GetModelMatrix()                   { return m_modelMatrix; }
    const uint64_t GetObjectId()                        { return m_objectId; }

private:
    uint64_t m_objectId = 0;
    uint64_t m_physicsId = 0;
    PickUpCreateInfo m_createInfo;
    Transform m_initialTransform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    MeshNodes m_meshNodes;
};