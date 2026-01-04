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
    void SetRotation(const glm::vec3& rotation);
    void Update(float deltaTime);
    void CleanUp();
    void Despawn();

    void SetDisabledPhysicsAtSpawnState(bool state);
    void SetRespawnState(bool state);

    const bool IsDespawned() const {return m_despawned;}

    MeshNodes& GetMeshNodes()                           { return m_meshNodes; }
    const bool IsDirty() const                          { return m_meshNodes.IsDirty(); }
    const PickUpCreateInfo& GetCreateInfo() const       { return m_createInfo; }
    const std::string GetName() const                   { return m_createInfo.name; }
    const ItemType GetType() const                    { return m_createInfo.type; }
    const std::vector<RenderItem>& GetRenderItems()     { return m_meshNodes.GetRenderItems(); }
    const glm::vec3 GetPosition()                       { return m_modelMatrix[3]; }
    const glm::vec3& GetRotation()                      { return m_initialTransform.rotation; }
    const glm::mat4& GetModelMatrix()                   { return m_modelMatrix; }
    const uint64_t GetObjectId()                        { return m_objectId; }
    const bool GetDisabledPhysicsAtSpawnState()         { return m_createInfo.disablePhysicsAtSpawn; }
    const bool GetRespawnState()                        { return m_createInfo.respawn; }

private:
    uint64_t m_objectId = 0;
    PickUpCreateInfo m_createInfo;
    Transform m_initialTransform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    MeshNodes m_meshNodes;
    bool m_firstFrame = true;
    float m_respawnCounter = 0;
    bool m_despawned = false;
};