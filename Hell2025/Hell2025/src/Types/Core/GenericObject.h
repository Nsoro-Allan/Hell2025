#pragma once
#include "CreateInfo.h"
#include "Managers/OpenableManager.h"
#include "Types/Renderer/MeshNodes.h"

struct GenericObject {
    GenericObject() = default;
    GenericObject(uint64_t id, const GenericObjectCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    GenericObject(const GenericObject&) = delete;
    GenericObject& operator=(const GenericObject&) = delete;
    GenericObject(GenericObject&&) noexcept = default;
    GenericObject& operator=(GenericObject&&) noexcept = default;
    ~GenericObject() = default;

    void Update(float deltaTime);
    void CleanUp();
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);

    glm::vec3 GetGizmoOffset();

    uint64_t GetObjectId()                                  { return m_objectId; }
    MeshNodes& GetMeshNodes()                               { return m_meshNodes; }
    bool IsDirty() const                                    { return m_meshNodes.IsDirty(); }
    const std::string& GetEditorName() const                { return m_createInfo.editorName; }
    const glm::vec3& GetPosition() const                    { return m_transform.position; }
    const glm::vec3& GetRotation() const                    { return m_transform.rotation; }
    const GenericObjectCreateInfo& GetCreateInfo() const    { return m_createInfo; }
    const std::vector<RenderItem>& GetRenderItems() const   { return m_meshNodes.GetRenderItems(); }
    const std::vector<uint64_t>& GetOpenableIds() const     { return m_openableIds; }

private:
    GenericObjectCreateInfo m_createInfo;
    Transform m_transform;
    MeshNodes m_meshNodes;
    uint64_t m_objectId;
    std::vector<uint64_t> m_openableIds;
};