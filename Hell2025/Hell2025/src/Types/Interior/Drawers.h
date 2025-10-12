#pragma once
#include "CreateInfo.h"
#include "Managers/OpenableManager.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"

struct Drawers {
    Drawers() = default;
    Drawers(DrawersCreateInfo createInfo, SpawnOffset spawnOffset);
    void Update(float deltaTime);
    void CleanUp();

    MeshNodes& GetMeshNodes()                               { return m_meshNodes; }
    bool MovedThisFrame() const                             { return m_movedThisFrame; }
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