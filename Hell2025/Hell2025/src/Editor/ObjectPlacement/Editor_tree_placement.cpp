#include "Editor/Editor.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include "Util/Util.h"

namespace Editor {
    TreeType g_treeType = TreeType::TREE_LARGE_0;

    void SetPlantType(TreeType treeType) {
        g_treeType = treeType;
    }

    void UpdateTreePlacement() {
        if (Input::LeftMousePressed()) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
            if (!viewport) return;

            // Cast physx ray
            float maxRayDistance = 2000;
            glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
            glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
            PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);

            // Place Tree
            if (rayResult.userData.physicsType == PhysicsType::HEIGHT_FIELD) {
                Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                TreeCreateInfo createInfo;
                createInfo.position = rayResult.hitPosition;
                createInfo.rotation.y = Util::RandomFloat(0.0f, HELL_PI * 2.0f);
                createInfo.type = g_treeType;
                createInfo.editorName = GetNextAvailableTreeName(g_treeType);

                if (g_treeType == TreeType::BLACK_BERRIES) {
                    createInfo.scale = glm::vec3(2.0f);
                }

                World::AddTree(createInfo);
                ExitObjectPlacement();
            }
            else {
                std::cout << "Failed to place tree. ";
                std::cout << "Hit found: " << Util::BoolToString(rayResult.hitFound) << " ";
                std::cout << "Hit position : " << rayResult.hitPosition << " ";
                std::cout << "Object type : " << Util::ObjectTypeToString(rayResult.userData.objectType) << " ";
                std::cout << "Physics type : " << Util::PhysicsTypeToString(rayResult.userData.physicsType) << " ";
                std::cout << "\n";
            }
        }

        // Exit placement
        if (Input::RightMouseDown()) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            ExitObjectPlacement();
        }
    }
}