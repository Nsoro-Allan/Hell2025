#pragma once
#include "CreateInfo.h"
#include "Types/Core/Openable.h"
#include "Types/Renderer/MeshNodes.h"
#include "Types/Renderer/Model.h"

struct Toilet {
    Toilet() = default;
    Toilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset);
    void Update(float deltaTime);
    void CleanUp();
    void InteractWithSeat();
    void InteractWithLid();

    const bool MovedThisFrame() const { return  m_movedThisFrame; }

    const std::vector<RenderItem>& GetRenderItems() const { return m_meshNodes.GetRenderItems(); }
    const MeshNodes& GetMeshNodes() const                 { return m_meshNodes; }

    Openable m_lidOpenHandler;
    Openable m_seatOpenHandler;

private:
    ToiletCreateInfo m_createInfo;
    Transform m_transform;
    MeshNodes m_meshNodes;
    bool m_movedThisFrame = false;

};