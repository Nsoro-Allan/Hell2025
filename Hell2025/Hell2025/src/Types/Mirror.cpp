#pragma once
#include "Mirror.h"
#include "AssetManagement/AssetManager.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "HellLogging.h"

Mirror::Mirror(uint64_t id, uint64_t parentId, uint32_t meshNodeIndex, uint32_t globalMeshIndex) {
    Mesh* mesh = AssetManager::GetMeshByIndex(globalMeshIndex);
    m_objectId = id;
    m_parentId = parentId;
    m_meshNodeIndex = meshNodeIndex;
    m_globalMeshIndex = globalMeshIndex;

    // Validity checks
    if (!mesh) {
        Logging::Error() << "Mirror::Mirror(..) failed because globalMeshIndex '" << globalMeshIndex << "' returned nullptr";
        return;
    }

    std::vector<Vertex> vertices = AssetManager::GetMeshVertices(mesh);
    if (vertices.empty()) {
        Logging::Error() << "Mirror::Mirror(..) failed because mesh has no vertices";
        return;
    }

    // Store AABB
    m_localAabb = AABB(mesh->aabbMin, mesh->aabbMax);

    // Compute local corners
    glm::vec3 boundsMin = mesh->aabbMin;
    glm::vec3 boundsMax = mesh->aabbMax;
    glm::vec3 size = boundsMax - boundsMin;

    int thinAxis = 0;
    if (size.y < size.x && size.y < size.z) thinAxis = 1;
    else if (size.z < size.x && size.z < size.y) thinAxis = 2;

    m_localCorners.clear();
    m_worldCorners.clear();

    if (thinAxis == 0) {
        float x = boundsMin.x;
        m_localCorners.push_back({ x, boundsMin.y, boundsMin.z });
        m_localCorners.push_back({ x, boundsMax.y, boundsMin.z });
        m_localCorners.push_back({ x, boundsMin.y, boundsMax.z });
        m_localCorners.push_back({ x, boundsMax.y, boundsMax.z });
    }
    else if (thinAxis == 1) {
        float y = boundsMin.y;
        m_localCorners.push_back({ boundsMin.x, y, boundsMin.z });
        m_localCorners.push_back({ boundsMax.x, y, boundsMin.z });
        m_localCorners.push_back({ boundsMin.x, y, boundsMax.z });
        m_localCorners.push_back({ boundsMax.x, y, boundsMax.z });
    }
    else {
        float z = boundsMin.z;
        m_localCorners.push_back({ boundsMin.x, boundsMin.y, z });
        m_localCorners.push_back({ boundsMax.x, boundsMin.y, z });
        m_localCorners.push_back({ boundsMin.x, boundsMax.y, z });
        m_localCorners.push_back({ boundsMax.x, boundsMax.y, z });
    }

    // Compute local center
    m_localCenter = (boundsMin + boundsMax) * glm::vec3(0.5f);

    // Local normal
    m_localNormal = glm::normalize(vertices[0].normal);
}

void Mirror::Update(const glm::mat4& worldMatrix ) {
    m_worldCenter = glm::vec3(worldMatrix * glm::vec4(m_localCenter, 1.0f)); 
    m_worldNormal = glm::normalize(glm::vec3(worldMatrix * glm::vec4(m_localNormal, 0.0f)));

    m_worldCorners.clear();

    for (const glm::vec3& localCorner : m_localCorners) {
        m_worldCorners.emplace_back(glm::vec3(worldMatrix * glm::vec4(localCorner, 1.0f)));
    }

    for (int i = 0; i < 4; i++) {
        m_clipPlanes[i] = glm::vec4(0.0f);
        m_reflectVectors[i] = glm::vec3(0.0f);
        m_viewMatrices[i] = glm::mat4(1.0f);

        if (Viewport* viewport = ViewportManager::GetViewportByIndex(i)) {
            if (!viewport->IsVisible()) continue;

            Player* player = Game::GetLocalPlayerByIndex(i);
            if (!player) continue;
            
            const glm::vec3& cameraPosition = player->GetCameraPosition();
            const glm::vec3& cameraUp = player->GetCameraUp();
            const glm::vec3& cameraForward = player->GetCameraForward();;
            
            constexpr float kMirrorPlaneEpsilon = 1e-4f;
            glm::vec4 planeWorld(m_worldNormal, -glm::dot(m_worldNormal, m_worldCenter) + kMirrorPlaneEpsilon);

            float signedDistanceFromCamera = glm::dot(m_worldNormal, cameraPosition) + planeWorld.w;
            glm::vec3 mirrorCameraPos = cameraPosition - 2.0f * signedDistanceFromCamera * m_worldNormal;
            glm::vec3 R = cameraForward - 2.0f * glm::dot(m_worldNormal, cameraForward) * m_worldNormal;
            
            // Guard against camera almost perpendicular to mirror
            float len2 = glm::dot(R, R);
            if (len2 < 1e-6f) {
                R = cameraForward;
            }
            else {
                R = glm::normalize(R);
            }

            glm::vec3 reflectedUp = cameraUp - 2.0f * glm::dot(m_worldNormal, cameraUp) * m_worldNormal;
            glm::vec3 right = glm::normalize(glm::cross(R, reflectedUp));

            reflectedUp = glm::normalize(glm::cross(right, R));

            m_clipPlanes[i] = planeWorld;
            m_reflectVectors[i] = R;
            m_viewMatrices[i] = glm::lookAt(mirrorCameraPos, mirrorCameraPos + R, reflectedUp);

            // Compute new projection matrix with smaller far plane
            const Resolutions& resolutions = Config::GetResolutions();
            int renderTargetWidth = resolutions.gBuffer.x;
            int renderTargetHeight = resolutions.gBuffer.y;
            float viewportWidth = viewport->GetSize().x * renderTargetWidth;
            float viewportHeight = viewport->GetSize().y * renderTargetHeight;
            float aspect = viewportWidth / viewportHeight;
            m_projectionMatrices[i] = glm::perspective(viewport->GetPerspectiveFOV(), aspect, viewport->GetNearPlane(), m_farDistance);

            // Update frustum for frustum culling
            m_frustums[i].Update(m_projectionMatrices[i] * m_viewMatrices[i]);

            // Is this mirror facing away from this viewport's camera?
            glm::vec3 toCamera = cameraPosition - m_worldCenter;
            m_facingViewportCamera[i] = glm::dot(m_worldNormal, toCamera) > 0.0f;
        }
    }
}

void Mirror::DebugDraw() {
    if (m_worldCorners.size() != 4) {
        Logging::Error() << "Mirror::DebugDraw() failed: mirror doesn't have four corners";
        return;
    }

    for (const glm::vec3& point : m_worldCorners) {
        Renderer::DrawPoint(point, OUTLINE_COLOR);
    }

    if (!IsFacingViewportCamera(0)) {
        Renderer::DrawLine(m_worldCorners[0], m_worldCorners[1], RED); // Left
        Renderer::DrawLine(m_worldCorners[2], m_worldCorners[3], RED); // Right
        Renderer::DrawLine(m_worldCorners[1], m_worldCorners[3], RED); // Top
        Renderer::DrawLine(m_worldCorners[0], m_worldCorners[2], RED); // Bottom
    }
    else {
        Renderer::DrawLine(m_worldCorners[0], m_worldCorners[1], WHITE); // Left
        Renderer::DrawLine(m_worldCorners[2], m_worldCorners[3], WHITE); // Right
        Renderer::DrawLine(m_worldCorners[1], m_worldCorners[3], WHITE); // Top
        Renderer::DrawLine(m_worldCorners[0], m_worldCorners[2], WHITE); // Bottom
    }

    // World normal
    glm::vec3 p1 = m_worldCenter + (m_worldNormal * 0.1f);
    Renderer::DrawPoint(m_worldCenter, OUTLINE_COLOR);
    Renderer::DrawPoint(p1, OUTLINE_COLOR);
    Renderer::DrawLine(m_worldCenter, p1, WHITE);
}

bool Mirror::IsFacingViewportCamera(int viewportIndex) const {
    if (viewportIndex < 0 || viewportIndex >= 4) return false;
    return m_facingViewportCamera[viewportIndex];

}
const glm::vec4& Mirror::GetClipPlane(int viewportIndex) const {
    static const glm::vec4 out_of_range = glm::vec4(0.0f);
    if (viewportIndex < 0 || viewportIndex >= 4) return out_of_range;
    return m_clipPlanes[viewportIndex];

}

const glm::vec3& Mirror::GetReflectVector(int viewportIndex) const {
    static const glm::vec3 out_of_range = glm::vec3(0.0f);
    if (viewportIndex < 0 || viewportIndex >= 4) return out_of_range;
    return m_reflectVectors[viewportIndex];
}

const glm::mat4& Mirror::GetProjecionMatrix(int viewportIndex) const {
    static const glm::mat4 out_of_range(1.0f);
    if (viewportIndex < 0 || viewportIndex >= 4) return out_of_range;
    return m_projectionMatrices[viewportIndex];
}

const glm::mat4& Mirror::GetViewMatrix(int viewportIndex) const {
    static const glm::mat4 out_of_range(1.0f);
    if (viewportIndex < 0 || viewportIndex >= 4) return out_of_range;
    return m_viewMatrices[viewportIndex];
}

Frustum* Mirror::GetFrustum(int viewportIndex) {
    if (viewportIndex < 0 || viewportIndex >= 4) return nullptr;
    return &m_frustums[viewportIndex];
}