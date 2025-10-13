#pragma once
#include "CreateInfo.h"
#include "Managers/OpenableManager.h"
#include "Types/Renderer/MeshNodes.h"

struct Drawers {
    Drawers() = default;
    Drawers(uint64_t id, const DrawersCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    Drawers(const Drawers&) = delete;
    Drawers& operator=(const Drawers&) = delete;
    Drawers(Drawers&&) noexcept = default;
    Drawers& operator=(Drawers&&) noexcept = default;
    ~Drawers() = default;

    void Update(float deltaTime);
    void CleanUp();
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);

    glm::vec3 GetGizmoOffset();

    uint64_t GetObjectId()                                  { return m_objectId; }
    MeshNodes& GetMeshNodes()                               { return m_meshNodes; }
    bool MovedThisFrame() const                             { return m_movedThisFrame; }
    const glm::vec3& GetPosition() const                    { return m_transform.position; }
    const glm::vec3& GetRotation() const                    { return m_transform.rotation; }
    const DrawersCreateInfo& GetCreateInfo() const          { return m_createInfo; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_meshNodes.GetRenderItems(); }
    const std::vector<uint64_t>& GetOpenableIds() const     { return m_openableIds; }

private:
    DrawersCreateInfo m_createInfo;
    Transform m_transform;
    MeshNodes m_meshNodes;
    uint64_t m_objectId;
    bool m_movedThisFrame = false;
    std::vector<uint64_t> m_openableIds;
};