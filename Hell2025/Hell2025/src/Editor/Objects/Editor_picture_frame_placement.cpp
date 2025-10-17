#include "Editor/Editor.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Viewport/ViewportManager.h"
#include "Util.h"

namespace Editor {
    void UpdatePictureFramePlacement() {
        if (Input::LeftMousePressed()) {
            // Bail if there is no hovered viewport
            Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
            if (!viewport) return;

            // Cast physx ray
            float maxRayDistance = 2000;
            glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
            glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
            PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);

            // Place picture frame
            if (rayResult.userData.objectType == ObjectType::WALL_SEGMENT) {
                Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                PictureFrameCreateInfo createInfo;
                createInfo.position = rayResult.hitPosition;
                createInfo.rotation = Util::EulerRotationFromNormal(rayResult.hitNormal);
                World::AddPictureFrame(createInfo);
                ExitObjectPlacement();
            }
        }

        // Exit placement
        if (Input::RightMouseDown()) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            ExitObjectPlacement();
        }
    }
}