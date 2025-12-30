#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Types/Renderer/MeshNodes.h"

struct Mermaid {
    Mermaid() = default;
    void Init(MermaidCreateInfo createInfo, SpawnOffset spawnOffset);
    void Update(float deltaTime);
    void CleanUp();

    const MermaidCreateInfo& GetCreateInfo() const                          { return m_createInfo; }
    const glm::vec3& GetPosition() const                                    { return m_transform.position; }
    const glm::vec3& GetLocalForward() const                                { return m_localForward; }
    const glm::vec3& GetWorldForward() const                                { return m_worldForward; }
    const std::vector<RenderItem>& GetRenderItems() const                   { return m_meshNodes.GetRenderItems(); }
    const std::vector<RenderItem>& GetRenderItemsBlended()const             { return m_meshNodes.GetRenderItemsBlended(); }
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() const     { return m_meshNodes.GetRenderItemsAlphaDiscarded(); }
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer() const       { return m_meshNodes.GetRenderItemsHairTopLayer(); }
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer() const    { return m_meshNodes.GetRenderItemsHairBottomLayer(); }

private:
    void UpdateRenderItems();
    void DebugDraw();

    SpawnOffset m_spawnOffset;
    MermaidCreateInfo m_createInfo;
    Transform m_transform;
    MeshNodes m_meshNodes;
    glm::vec3 m_localForward = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 m_worldForward = glm::vec3(0.0f);
};