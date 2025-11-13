
#include "Audio/Audio.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "World/World.h"
#include "Viewport/ViewportManager.h"

namespace Editor {
    uint64_t g_placementObjectId = 0;
    EditorState g_lastPlacementState = EditorState::IDLE;

    //void PlaceObject(ObjectType objectType, glm::vec3 hitPosition, glm::vec3 hitNormal);
    void PlaceHousePlane(HousePlaneType housePlaneType, glm::vec3 hitPosition, glm::vec3 hitNormal);
    void PlaceGenericObject(GenericObjectType genericObjectType, glm::vec3 hitPosition, glm::vec3 hitNormal);

    void UpdateObjectPlacement() {
        if (GetEditorState() == EditorState::PLACE_OBJECT) {

            if (Input::LeftMousePressed()) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
                if (!viewport) return;

                // Cast physx ray
                float maxRayDistance = 2000;
                glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
                glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
                PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);

                // u gotta fix this later, its a mess
                if (rayResult.hitFound) {

                    if (GetPlacementObjectType() == ObjectType::GENERIC_OBJECT) {
                        PlaceGenericObject(GetPlacementObjectSubtype().genericObject, rayResult.hitPosition, rayResult.hitNormal);
                    }
                    if (GetPlacementObjectType() == ObjectType::HOUSE_PLANE) {
                        PlaceHousePlane(GetPlacementObjectSubtype().housePlane, rayResult.hitPosition, rayResult.hitNormal);
                    }
                }
            }
        }
        if (GetEditorState() == EditorState::PLACE_DOOR) UpdateDoorPlacement();
        //if (GetEditorState() == EditorState::PLACE_HOUSE) UpdateDoorPlacement();
        //if (GetEditorState() == EditorState::PLACE_PICTURE_FRAME) UpdatePictureFramePlacement();
        //if (GetEditorState() == EditorState::PLACE_TREE) UpdateTreePlacement();
        if (GetEditorState() == EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN) UpdatePlayerCampaignSpawnPlacement();
        if (GetEditorState() == EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN) UpdatePlayerDeathmatchSpawnPlacement();
        if (GetEditorState() == EditorState::PLACE_WALL) UpdateWallPlacement();
        if (GetEditorState() == EditorState::PLACE_WINDOW) UpdateWindowPlacement();
        //
        //// Store last placement state
        //if (GetEditorState() == EditorState::PLACE_DOOR ||
        //    GetEditorState() == EditorState::PLACE_PICTURE_FRAME ||
        //    GetEditorState() == EditorState::PLACE_TREE ||
        //    GetEditorState() == EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN ||
        //    GetEditorState() == EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN ||
        //    GetEditorState() == EditorState::PLACE_WINDOW) {
        //    g_lastPlacementState = GetEditorState();
        //}

        // Re-insert last
        if (Input::KeyDown(HELL_KEY_LEFT_CONTROL_GLFW) && Input::KeyPressed(HELL_KEY_T)) {

            // House editor
            if (GetEditorMode() == EditorMode::HOUSE_EDITOR) {
                if (g_lastPlacementState == EditorState::PLACE_PICTURE_FRAME) {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    SetEditorState(EditorState::PLACE_PICTURE_FRAME);
                }
            }
            // Sector editor
            if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
                if (g_lastPlacementState == EditorState::PLACE_TREE) {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    SetEditorState(EditorState::PLACE_TREE);
                }
                if (g_lastPlacementState == EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN) {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    SetEditorState(EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN);
                }
                if (g_lastPlacementState == EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN) {
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    SetEditorState(EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN);
                }
            }
        }
    }

    //void PlaceObject(ObjectType objectType, glm::vec3 hitPosition, glm::vec3 hitNormal) {
    //    if (objectType == ObjectType::GENERIC_OBJECT) {
    //        GenericObjectCreateInfo createInfo;
    //        createInfo.position = hitPosition;
    //        createInfo.rotation.y = 0.0f;
    //        createInfo.type = GenericObjectType::DRAWERS_LARGE;
    //        World::AddGenericObject(createInfo, SpawnOffset());
    //        ExitObjectPlacement();
    //        return;
    //    }
    //}


    void PlaceHousePlane(HousePlaneType housePlaneType, glm::vec3 hitPosition, glm::vec3 hitNormal) {
        HousePlaneCreateInfo createInfo; 
        createInfo.p0 = hitPosition + glm::vec3(-1.0f, 0.0f, -1.0f);
        createInfo.p1 = hitPosition + glm::vec3(-1.0f, 0.0f, 1.0f);
        createInfo.p2 = hitPosition + glm::vec3(1.0f, 0.0f, 1.0f);
        createInfo.p3 = hitPosition + glm::vec3(1.0f, 0.0f, -1.0f);
        createInfo.type = housePlaneType;
        World::AddHousePlane(createInfo, SpawnOffset()); 
        World::UpdateHouseMeshBuffer();
        ExitObjectPlacement();
    }

    void PlaceGenericObject(GenericObjectType genericObjectType, glm::vec3 hitPosition, glm::vec3 hitNormal) {
        GenericObjectCreateInfo createInfo;
        createInfo.position = hitPosition;
        createInfo.rotation.y = 0.0f;
        createInfo.type = genericObjectType;
        World::AddGenericObject(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void ExitObjectPlacement() {
        SetPlacementObjectId(0);
        SetEditorState(EditorState::IDLE);
        UpdateOutliner();
    }

    uint64_t GetPlacementObjectId() {
        return g_placementObjectId;
    }

    void SetPlacementObjectId(uint64_t objectId) {
        g_placementObjectId = objectId;
    }

}