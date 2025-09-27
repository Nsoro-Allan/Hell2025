#include "Wire.h"
#include "Renderer/Renderer.h"
#include "Util.h"

void Wire::Init(glm::vec3 begin, glm::vec3 end, float sag, float radius) {
    m_begin = begin;
    m_end = end;
    m_sag = sag;
    m_radius = radius;
    float span = glm::distance(m_begin, m_end);
    float numPoints = span * 2;
    m_segmentPoints = Util::GenerateSagPoints(m_begin, m_end, numPoints, m_sag);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int j = 0; j < m_segmentPoints.size() - 1; j++) {
        glm::vec3& p0 = m_segmentPoints[j];
        glm::vec3& p1 = m_segmentPoints[j + 1];
        glm::vec3 forward = p0 - p1;
        const std::vector<glm::vec3>& circle1 = Util::GenerateCirclePoints(p0, forward, m_radius, numPoints);
        const std::vector<glm::vec3>& circle2 = Util::GenerateCirclePoints(p1, forward, m_radius, numPoints);
        size_t pointCount = circle1.size();
        glm::vec3 center1(0.0f), center2(0.0f);
        for (const auto& p : circle1) center1 += p;
        for (const auto& p : circle2) center2 += p;
        center1 /= (float)pointCount;
        center2 /= (float)pointCount;

        for (size_t i = 0; i < pointCount; ++i) {
            size_t next = (i + 1) % pointCount;
            // Positions
            glm::vec3 pos1 = circle1[i];
            glm::vec3 pos2 = circle1[next];
            glm::vec3 pos3 = circle2[i];
            glm::vec3 pos4 = circle2[next];
            // Normals
            glm::vec3 normal1 = glm::normalize(pos1 - center1);
            glm::vec3 normal2 = glm::normalize(pos2 - center1);
            glm::vec3 normal3 = glm::normalize(pos3 - center2);
            glm::vec3 normal4 = glm::normalize(pos4 - center2);
            // UVs
            glm::vec2 uv1(i / (float)pointCount, 0.0f);
            glm::vec2 uv2((next) / (float)pointCount, 0.0f);
            glm::vec2 uv3(i / (float)pointCount, 1.0f);
            glm::vec2 uv4((next) / (float)pointCount, 1.0f);
            // Tangents
            glm::vec3 edge1 = pos2 - pos1;
            glm::vec3 edge2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            glm::vec3 tangent1;
            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent1 = glm::normalize(tangent1);
            edge1 = pos4 - pos2;
            edge2 = pos3 - pos2;
            deltaUV1 = uv4 - uv2;
            deltaUV2 = uv3 - uv2;
            glm::vec3 tangent2;
            tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent2 = glm::normalize(tangent2);
            // Populate vectors
            uint32_t idx1 = (uint32_t)vertices.size();
            vertices.emplace_back(pos1, normal1, uv1, tangent1);
            vertices.emplace_back(pos2, normal2, uv2, tangent1);
            vertices.emplace_back(pos3, normal3, uv3, tangent2);
            vertices.emplace_back(pos4, normal4, uv4, tangent2);
            indices.push_back(idx1);
            indices.push_back(idx1 + 1);
            indices.push_back(idx1 + 2);
            indices.push_back(idx1 + 1);
            indices.push_back(idx1 + 3);
            indices.push_back(idx1 + 2);
        }
    }

    m_meshBuffer.AddMesh(vertices, indices);
    m_meshBuffer.UpdateBuffers();
}

void Wire::Update() {
    for (glm::vec3& point : m_segmentPoints) {
        Renderer::DrawPoint(point, OUTLINE_COLOR);
    }
}

