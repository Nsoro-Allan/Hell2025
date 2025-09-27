#include "Editor.h"
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
}