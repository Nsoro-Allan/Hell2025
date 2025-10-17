#pragma once
#include <glm/vec3.hpp>

struct AABB {
    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max);
    void Grow(AABB& b);
    void Grow(const glm::vec3& p);
    float Area();
    bool IntersectsSphere(const glm::vec3& sphereCenter, float radius) const;
    bool IntersectsAABB(const AABB& other) const;
    bool IntersectsAABB(const AABB& other, float threshold) const;
    bool ContainsPoint(const glm::vec3& point) const;
    glm::vec3 NearestPointTo(const glm::vec3& worldPosition) const;

    const glm::vec3& GetCenter()         const { return center; }
    const glm::vec3& GetBoundsMin()      const { return boundsMin; }
    const glm::vec3& GetBoundsMax()      const { return boundsMax; }
    const glm::vec3& GetExtents()        const { return extents; }

private:
    void CalculateCenterAndExtents();
    glm::vec3 extents = glm::vec3(0);
    glm::vec3 center = glm::vec3(0);
    glm::vec3 boundsMin = glm::vec3(1e30f);
    glm::vec3 boundsMax = glm::vec3(-1e30f);
};