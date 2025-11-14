#pragma once
#include "HellTypes.h"

struct NavTri {
    glm::vec3 v[3];     // World-space vertices (XZ plane for movement)
    int neighbor[3];    // Index of neighbor across each edge, or -1
};

namespace NavMesh {
    void CreateNavMesh();
    void RenderDebug();
}