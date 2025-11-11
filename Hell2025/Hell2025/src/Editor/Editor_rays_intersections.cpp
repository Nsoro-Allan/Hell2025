#include "Editor.h"
#include <glm/gtx/intersect.hpp>
#include "Physics/Physics.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    glm::vec3 GetMouseRayPlaneIntersectionPoint(glm::vec3 planeOrigin, glm::vec3 planeNormal) {
        int viewportIndex = Editor::GetHoveredViewportIndex();
        const Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        const glm::vec3 rayOrigin = Editor::GetMouseRayOriginByViewportIndex(viewportIndex);
        const glm::vec3 rayDir = Editor::GetMouseRayDirectionByViewportIndex(viewportIndex);
        float distanceToHit = 0;
        bool hitFound = glm::intersectRayPlane(rayOrigin, rayDir, planeOrigin, planeNormal, distanceToHit);

        if (!hitFound) {
            return glm::vec3(0.0f);
        }
        else {
            glm::vec3 hitPosition = rayOrigin + (rayDir * distanceToHit);

            // Snap to grid
            hitPosition = glm::round(hitPosition * 10.0f) / 10.0f;

            return hitPosition;
        }
    }
}