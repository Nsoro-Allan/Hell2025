#pragma once
#include "HellTypes.h"
#include <vector>

enum RoadCurveType {
    STRAIGHT,
    BEIZER
};

struct Road {
    std::vector<glm::vec2> m_controlPoints2D;
    std::vector<glm::vec3> m_controlPoints3D;
    std::vector<glm::vec3> m_worldPoints;

    void Init();
    void Update();
    void DrawPoints();
};