#pragma once
#include "HellTypes.h"
#include <vector>

enum RoadCurveType {
    STRAIGHT,
    BEIZER
};

struct Road {
    std::vector<glm::vec2> m_points;
    std::vector<glm::vec2> m_worldPoints;

    void Init();
    void DrawPoints();
};