#include "Editor.h"
#include "Config/Config.h"
#include "HellLogging.h"
#include "Physics/Physics.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    PhysXRayResult GetMouseRayPhsyXHitPosition() {
        Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
        if (!viewport) return PhysXRayResult();

        // Cast physx ray
        float maxRayDistance = 2000;
        glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
        glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
        return Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);
    }

    float GetScalingFactor(int targetSizeInPixels) {
        Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
        if (!viewport) return 0.0f;

        const Resolutions& resolutions = Config::GetResolutions();

        int renderTargetWidth = resolutions.gBuffer.x;
        int renderTargetHeight = resolutions.gBuffer.y;
        float viewportWidth = viewport->GetSize().x * renderTargetWidth;
        float viewportHeight = viewport->GetSize().y * renderTargetHeight;

        if (viewport->IsOrthographic()) {
            float m_aspect = viewportWidth / viewportHeight;
            float left = -viewport->GetOrthoSize() * m_aspect;
            float right = viewport->GetOrthoSize() * m_aspect;
            float bottom = -viewport->GetOrthoSize();
            float top = viewport->GetOrthoSize();
            float worldHeight = top - bottom;
            float worldPerPixel = worldHeight / viewportHeight;
            float gizmoHeightInWorld = (float)targetSizeInPixels * worldPerPixel;
            return gizmoHeightInWorld;
        }
        else {
            Logging::Error() << "Editor::GetScalingFactor() failed because viewport was not orthographic";
        }
    }
}