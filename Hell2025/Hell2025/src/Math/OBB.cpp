#include "OBB.h"

OBB::OBB(const AABB& bounds, const glm::mat4& matrix) {
    m_localBounds = bounds;
    m_worldTransform = matrix;
    RecomputeCorners();
}

void OBB::SetTransform(const glm::mat4& matrix) {
    m_worldTransform = matrix;
    RecomputeCorners();
}

void OBB::SetLocalBounds(const AABB& bounds) {
    m_localBounds = bounds;
    RecomputeCorners();
}

void OBB::RecomputeCorners() {
    m_corners.clear();
    m_corners.reserve(8);

    const glm::vec3& min = m_localBounds.GetBoundsMin();
    const glm::vec3& max = m_localBounds.GetBoundsMax();

    std::vector<glm::vec3> localPoints = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, max.y, max.z)
    };

    for (int i = 0; i < 8; i++) {
        glm::vec4 worldP = m_worldTransform * glm::vec4(localPoints[i], 1.0f);
        m_corners.push_back(glm::vec3(worldP));
    }
}