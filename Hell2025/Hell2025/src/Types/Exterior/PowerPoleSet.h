#pragma once
#include "HellTypes.h"
#include "Types/Generics/Wire.h"
#include "Types/Renderer/MeshNodes.h"

struct PowerPoleSet {
    void Init();
    void Update();
    void CleanUp();
    const std::vector<RenderItem>& const GetRenderItems();

    std::vector<Wire>& GetWires() { return m_wires; }

private:
    std::vector<glm::vec3> m_finalPositions;
    std::vector<glm::vec3> m_wirePositionsBackA;
    std::vector<glm::vec3> m_wirePositionsBackB;
    std::vector<glm::vec3> m_wirePositionsBackC;
    std::vector<glm::vec3> m_wirePositionsBackD;
    std::vector<glm::vec3> m_wirePositionsFrontA;
    std::vector<glm::vec3> m_wirePositionsFrontB;
    std::vector<glm::vec3> m_wirePositionsFrontC;
    std::vector<glm::vec3> m_wirePositionsFrontD;
    std::vector<RenderItem> m_renderItems;
    MeshNodes m_meshNodes; 
    
    std::vector<Wire> m_wires;
};