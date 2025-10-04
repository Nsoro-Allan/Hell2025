#pragma once
#include "HellTypes.h"
#include "Types/Generics/Wire.h"
#include "Types/Renderer/MeshNodes.h"

struct Fence {
    void Init();
    void Update();
    void CleanUp();

    const std::vector<RenderItem>& const GetRenderItems();

    std::vector<Wire>& GetWires() { return m_wires; }

private:
    RenderItem CreateWireRenderItem(RenderItem& localSpaceRenderItem, glm::vec3& position, glm::vec3 nextPosition);

    std::vector<glm::vec3> m_finalPositions;
    std::vector<glm::vec3> m_wirePositionsA;
    std::vector<glm::vec3> m_wirePositionsB;
    std::vector<glm::vec3> m_wirePositionsC;
    std::vector<glm::vec3> m_wirePositionsD;
    std::vector<glm::vec3> m_wirePositionsE;
    std::vector<RenderItem> m_renderItems;
    MeshNodes m_meshNodesFat;
    MeshNodes m_meshNodesThin;
    MeshNodes m_meshNodesWire;
    MeshNodes m_meshNodesWireBarbed;
    
    std::vector<Wire> m_wires;
};