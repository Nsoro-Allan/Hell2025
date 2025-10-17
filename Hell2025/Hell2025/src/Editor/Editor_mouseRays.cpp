#include "Editor.h"
#include "HellLogging.h"
#include "Util.h"
#include "Viewport/ViewportManager.h"

namespace Editor {
    glm::vec3 g_mouseRayOrigins[4];
    glm::vec3 g_mouseRayDirections[4];

    void UpdateMouseRays() {
        for (int i = 0; i < 4; i++) {
            //const Camera* camera = GetCameraByIndex(i);
            const Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            const SpaceCoords gBufferSpaceCoords = viewport->GetGBufferSpaceCoords();

            const glm::mat4 projectionMatrix = viewport->GetProjectionMatrix();
            //const glm::mat4 viewMatrix = camera->GetViewMatrix();

            const glm::mat4 viewMatrix = Editor::GetViewportViewMatrix(i);
            const glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;
            const glm::mat4 inverseProjectionViewMatrix = glm::inverse(projectionViewMatrix);
        
            // Orthographic
            if (viewport->IsOrthographic()) {
                //float ndcX = (2.0f * gBufferSpaceCoords.localMouseX / gBufferSpaceCoords.width) - 1.0f;
                //float ndcY = 1.0f - (2.0f * (gBufferSpaceCoords.localMouseY / gBufferSpaceCoords.height));
                //glm::vec4 nearClip(ndcX, ndcY, -1.0f, 1.0f);
                //glm::vec4 farClip(ndcX, ndcY, 1.0f, 1.0f);
                //glm::vec4 nearWorld = inverseProjectionViewMatrix * nearClip;
                //nearWorld /= nearWorld.w;
                //glm::vec4 farWorld = inverseProjectionViewMatrix * farClip;
                //farWorld /= farWorld.w;
                //g_mouseRayOrigins[i] = glm::vec3(nearWorld);
                //g_mouseRayDirections[i] = glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld));
                //
                //// Numerically stabilize
                //const glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
                //const glm::vec3 cameraWorldPos = glm::vec3(inverseViewMatrix[3]);
                //glm::vec3 vecToCam = cameraWorldPos - g_mouseRayOrigins[i];
                //float t = glm::dot(vecToCam, g_mouseRayDirections[i]);
                //glm::vec3 newRayOrigin = g_mouseRayOrigins[i] + t * g_mouseRayDirections[i];
                //g_mouseRayOrigins[i] = newRayOrigin;

                // Not correct but is working mostly
                float ndcX = (2.0f * gBufferSpaceCoords.localMouseX / gBufferSpaceCoords.width) - 1.0f;
                float ndcY = 1.0f - (2.0f * (gBufferSpaceCoords.localMouseY / gBufferSpaceCoords.height));
                glm::vec4 rayOriginNdc(ndcX, ndcY, 0.0f, 1.0f);
                glm::vec4 worldPoint = inverseProjectionViewMatrix * rayOriginNdc;
                g_mouseRayOrigins[i] = glm::vec3(worldPoint) / worldPoint.w;
                g_mouseRayDirections[i] = -glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
            }
            // Perspective
            else {
                Logging::Error() << "Editor::MouseRays() failed because the viewport was not orthographic.";
            }
        }
    }

    glm::vec3 GetMouseRayOriginByViewportIndex(int32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_mouseRayOrigins[viewportIndex];
        }
        else {
            std::cout << "Editor::GetMouseRayOriginByViewportIndex(int32_t viewportIndex) Failed because " << viewportIndex << " is out of range of size 4\n";
            return glm::vec3(0.0f, 0.0f, -1.0f);
        }
    }

    glm::vec3 GetMouseRayDirectionByViewportIndex(int32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_mouseRayDirections[viewportIndex];
        }
        else {
            std::cout << "Editor::GetMouseRayDirectionByViewportIndex(int32_t viewportIndex) Failed because " << viewportIndex << " is out of range of size 4\n";
            return glm::vec3(0.0f, 0.0f, -1.0f);
        }
    }
}