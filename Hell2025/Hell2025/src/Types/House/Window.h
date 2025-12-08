#pragma once
#include "CreateInfo.h"
#include "HellTypes.h"
#include "Types/Renderer/MeshNodes.h"

struct Window {
    Window() = default;
    Window(uint64_t id, const WindowCreateInfo& createInfo, const SpawnOffset& spawnOffset);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept = default;
    Window& operator=(Window&&) noexcept = default;
    ~Window() = default;

    void Update(float deltaTime);
    void CleanUp();
    void SetPosition(const glm::vec3& position);
    
    const uint64_t GetObjectId() const                          { return m_objectId; }
    const glm::vec3& GetPosition() const                        { return m_transform.position; }
    const glm::vec3& GetRotation() const                        { return m_transform.rotation; }
    const std::vector<RenderItem>& GetRenderItems() const       { return m_meshNodes.GetRenderItems(); }
    const std::vector<RenderItem>& GetGlassRenderItems() const  { return m_meshNodes.GetRenderItemsGlass(); }
    const WindowCreateInfo& GetCreateInfo() const               { return m_createInfo; }
    MeshNodes& GetMeshNodes()                                   { return m_meshNodes; }

private:
    uint64_t m_objectId = 0;
    uint64_t m_physicsId = 0;
    MeshNodes m_meshNodes;
    Transform m_transform;
    WindowCreateInfo m_createInfo;
};