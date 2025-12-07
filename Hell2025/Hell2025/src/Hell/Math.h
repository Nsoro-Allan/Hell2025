#pragma once
#include <glm/glm.hpp>

namespace Math {
    inline float DistSquared(const glm::vec3& a, const glm::vec3& b);
    inline float DistSquared2D(const glm::vec2& a, const glm::vec2& b);
    inline bool IsDegenerateXZ(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    inline bool PointsEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f);
    inline glm::vec3 SnapVec3(const glm::vec3& v, int decimalPlaces);
    inline float TriArea2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);


    inline float DistSquared(const glm::vec3& a, const glm::vec3& b) {
        glm::vec3 d = a - b;
        return glm::dot(d, d);
    }


    inline float DistSquared2D(const glm::vec2& a, const glm::vec2& b) {
        glm::vec2 d = a - b;
        return glm::dot(d, d);
    }


    inline bool IsDegenerateXZ(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        // Check if any two points are the same
        if (PointsEqual(a, b, 0.0001f) ||
            PointsEqual(b, c, 0.0001f) ||
            PointsEqual(c, a, 0.0001f)) {
            return true;
        }

        // Check for near-zero area (only care about the XZ plane)
        glm::vec2 A(a.x, a.z), B(b.x, b.z), C(c.x, c.z);
        float area2 = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);

        // Check if area is extremely small
        return std::fabs(area2) < 0.00001f;
    }


    inline bool PointsEqual(const glm::vec3& a, const glm::vec3& b, float epsilon) {
        glm::vec3 d = a - b;
        return d.x * d.x + d.y * d.y + d.z * d.z <= epsilon * epsilon;
    }


    inline glm::vec3 SnapVec3(const glm::vec3& v, int decimalPlaces) {
        static const float pow10[] = {
            1.0f,
            10.0f,
            100.0f,
            1000.0f,
            10000.0f,
            100000.0f
        };

        if (decimalPlaces < 0) decimalPlaces = 0;
        if (decimalPlaces > 5) decimalPlaces = 5;

        float scale = pow10[decimalPlaces];
        return glm::round(v * scale) / scale;
    }


    inline float TriArea2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }
}