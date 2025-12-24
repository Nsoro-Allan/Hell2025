
#include "Audio/Audio.h"
#include "Bible/Bible.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "World/World.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/Renderer.h"

namespace Editor {
    uint64_t g_placementObjectId = 0;
    EditorState g_lastPlacementState = EditorState::IDLE;

    //void PlaceObject(ObjectType objectType, glm::vec3 hitPosition, glm::vec3 hitNormal);
    void PlaceFireplace(FireplaceType fireplaceType, const glm::vec3& hitPosition);
    void PlaceHousePlane(HousePlaneType housePlaneType, const glm::vec3& hitPosition, const glm::vec3& hitNormal);
    void PlaceGenericObject(GenericObjectType genericObjectType, const glm::vec3& hitPosition, const glm::vec3& hitNormal);
    void PlacePickUp(const std::string& pickUpName, const glm::vec3& hitPosition, const glm::vec3& hitNormal);
    void PlaceDoor(const glm::vec3& hitPosition);
    void PlaceWindow(const glm::vec3& hitPosition);
    void PlaceLadder(const glm::vec3& hitPosition);
    void PlaceLight(const glm::vec3& hitPosition);
    void PlaceStaircase(const glm::vec3& hitPosition);

    void UpdateObjectPlacement() {
        Viewport* viewport = ViewportManager::GetViewportByIndex(GetHoveredViewportIndex());
        if (!viewport) return;

        // Cast physx ray
        float maxRayDistance = 2000;
        glm::vec3 rayOrigin = GetMouseRayOriginByViewportIndex(GetHoveredViewportIndex());
        glm::vec3 rayDir = GetMouseRayDirectionByViewportIndex(GetHoveredViewportIndex());
        //PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, true);

        PhysXRayResult physXRayResult = Physics::CastPhysXRay(rayOrigin, rayDir, maxRayDistance, Editor::BackfaceCullingEnabled());
        BvhRayResult bvhRayResult = World::ClosestHit(rayOrigin, rayDir, maxRayDistance);

        glm::vec3 hitPosition = glm::vec3(-9999.0f);
        glm::vec3 hitNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        bool hitFound = false;

        // Replace me with some distance check with closest point from hit object AABB
        if (bvhRayResult.hitFound) {
            hitFound = true;
            hitPosition = bvhRayResult.hitPosition;
            hitNormal = bvhRayResult.hitNormal;
        }

        // Now try see if the PhysX hit is closer
        if (physXRayResult.hitFound && physXRayResult.distanceToHit < bvhRayResult.distanceToHit) {
            hitFound = true;
            hitPosition = physXRayResult.hitPosition;
            hitNormal = physXRayResult.hitNormal;
        }

        // Simple objects
        if (GetEditorState() == EditorState::PLACE_OBJECT) {

            if (Input::LeftMousePressed() && hitFound) {

                if (GetPlacementObjectType() == ObjectType::PICK_UP) {
                    PlacePickUp(GetPlacementObjectSubtype().pickUpName, hitPosition, hitNormal);
                }
                if (GetPlacementObjectType() == ObjectType::GENERIC_OBJECT) {
                    PlaceGenericObject(GetPlacementObjectSubtype().genericObject, hitPosition, hitNormal);
                }
                if (GetPlacementObjectType() == ObjectType::HOUSE_PLANE) {
                    PlaceHousePlane(GetPlacementObjectSubtype().housePlane, hitPosition, hitNormal);
                }
                if (GetPlacementObjectType() == ObjectType::FIREPLACE) {
                    PlaceFireplace(GetPlacementObjectSubtype().fireplace, hitPosition);
                }
                if (GetPlacementObjectType() == ObjectType::DOOR) {
                    PlaceDoor(hitPosition);
                }
                if (GetPlacementObjectType() == ObjectType::WINDOW) {
                    PlaceWindow(hitPosition);
                }
                if (GetPlacementObjectType() == ObjectType::LADDER) {
                    PlaceLadder(hitPosition);
                }
                if (GetPlacementObjectType() == ObjectType::LIGHT) {
                    PlaceLight(hitPosition);
                }
                if (GetPlacementObjectType() == ObjectType::STAIRCASE) {
                    PlaceStaircase(hitPosition);
                }
            }
        }

        // Christmas lights
        if (GetEditorState() == EditorState::PLACE_CHRISTMAS_LIGHTS) {

            static glm::vec3 lastPoint = glm::vec3(0.0f);
            static float defaultSag = 0.3f;
            static float currentSag = defaultSag;

            if (Input::LeftMousePressed() && hitFound) {

                // Create first point
                if (g_placementObjectId == 0) {
                    ChristmasLightsCreateInfo createInfo;
                    createInfo.position = hitPosition;
                    createInfo.spiral = false;
                    lastPoint = hitPosition;
                    currentSag = defaultSag;

                    g_placementObjectId = World::AddChristmasLights(createInfo, SpawnOffset());
                    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                }
                else {
                    if (ChristmasLights* christmasLights = World::GetChristmasLightsByObjectId(g_placementObjectId)) {
                        christmasLights->AddSegementFromLastPoint(hitPosition, currentSag);
                        lastPoint = hitPosition;
                        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                    }
                }
            }
            if (Input::RightMousePressed()) {
                Audio::PlayAudio(AUDIO_SELECT, 1.0f);
                ExitObjectPlacement();
            }

            // Sag adjust
            if (g_placementObjectId != 0) {
                float size = 0.02f;
                if (Input::MouseWheelUp()) {
                    currentSag -= size;
                }
                if (Input::MouseWheelDown()) {
                    currentSag += size;
                }
            }

            // Draw line
            if (g_placementObjectId != 0) {
                if (ChristmasLights* christmasLights = World::GetChristmasLightsByObjectId(g_placementObjectId)) {

                    //float sag = christmasLights->GetCreateInfo().sagHeights;
                    glm::vec3 begin = lastPoint;
                    glm::vec3 end = hitPosition;
                    float spacing = christmasLights->GetCreateInfo().spacing;

                    const float span = glm::distance(begin, end);
                    const float minSpan = 0.0001f;
                    if (span > minSpan) {
                        if (spacing <= 0.0f) {
                            spacing = 0.25f; // Sensible default
                        }
                        // Number of points along the sag polyline, including endpoints
                        const int segmentCount = std::max(1, (int)std::ceil(span / spacing));
                        const int numSagPoints = segmentCount + 1;

                        std::vector<glm::vec3> sagPoints = Util::GenerateSagPoints(begin, end, numSagPoints, currentSag);
                        for (int i = 0; i < sagPoints.size() - 1; i++) {
                            const glm::vec3 p1 = sagPoints[i];
                            const glm::vec3 p2 = sagPoints[i + 1];
                            Renderer::DrawLine(p1, p2, BLACK);
                        }
                    }
                }
            }
        }

        if (GetEditorState() == EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN) UpdatePlayerCampaignSpawnPlacement();
        if (GetEditorState() == EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN) UpdatePlayerDeathmatchSpawnPlacement();
        if (GetEditorState() == EditorState::PLACE_WALL) UpdateWallPlacement();
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

    void PlaceFireplace(FireplaceType fireplaceType, const glm::vec3& hitPosition) {
        FireplaceCreateInfo createInfo;
        createInfo.position = hitPosition;
        createInfo.type = fireplaceType;
        World::AddFireplace(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlaceHousePlane(HousePlaneType housePlaneType, const glm::vec3& hitPosition, const glm::vec3& hitNormal) {
        HousePlaneCreateInfo createInfo;

        if (housePlaneType == HousePlaneType::FLOOR) {
            createInfo.p0 = hitPosition + glm::vec3(-1.0f, 0.0f, -1.0f);
            createInfo.p1 = hitPosition + glm::vec3(-1.0f, 0.0f, 1.0f);
            createInfo.p2 = hitPosition + glm::vec3(1.0f, 0.0f, 1.0f);
            createInfo.p3 = hitPosition + glm::vec3(1.0f, 0.0f, -1.0f);
        }
        if (housePlaneType == HousePlaneType::CEILING) {
            float h = 2.4f;
            createInfo.p3 = hitPosition + glm::vec3(-1.0f, h, -1.0f);
            createInfo.p2 = hitPosition + glm::vec3(-1.0f, h, 1.0f);
            createInfo.p1 = hitPosition + glm::vec3(1.0f, h, 1.0f);
            createInfo.p0 = hitPosition + glm::vec3(1.0f, h, -1.0f);
        }

        createInfo.type = housePlaneType;

        World::AddHousePlane(createInfo, SpawnOffset());
        World::UpdateHouseMeshBuffer();
        ExitObjectPlacement();
    }

    void PlaceGenericObject(GenericObjectType genericObjectType, const glm::vec3& hitPosition, const glm::vec3& hitNormal) {
        GenericObjectCreateInfo createInfo;
        createInfo.position = hitPosition;
        createInfo.rotation.y = 0.0f;
        createInfo.type = genericObjectType;
        World::AddGenericObject(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlacePickUp(const std::string& pickUpName, const glm::vec3& hitPosition, const glm::vec3& hitNormal) {
        PickUpCreateInfo createInfo;;
        createInfo.position = hitPosition + glm::vec3(0.0, 0.5f, 0.0f);
        createInfo.rotation.y = 0.0f;
        createInfo.name = pickUpName;
        createInfo.respawn = true;
        createInfo.saveToFile = true;
        createInfo.type = Bible::GetPickUpTypeByName(pickUpName);
        World::AddPickUp(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlaceDoor(const glm::vec3& hitPosition) {
        DoorCreateInfo createInfo;
        createInfo.type = DoorType::STANDARD_A;
        createInfo.materialTypeFront = DoorMaterialType::RESIDENT_EVIL;
        createInfo.materialTypeBack = DoorMaterialType::RESIDENT_EVIL;
        createInfo.materialTypeFrameFront = DoorMaterialType::RESIDENT_EVIL;
        createInfo.materialTypeFrameBack = DoorMaterialType::RESIDENT_EVIL;
        createInfo.position = hitPosition;
        createInfo.rotation = glm::vec3(0.0f);
        createInfo.hasDeadLock = false;
        createInfo.deadLockedAtInit = false;
        createInfo.maxOpenValue = 2.1f;
        createInfo.editorName = UNDEFINED_STRING;
        World::AddDoor(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlaceWindow(const glm::vec3& hitPosition) {
        WindowCreateInfo createInfo;
        createInfo.position = hitPosition;
        createInfo.rotation = glm::vec3(0.0f);
        World::AddWindow(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlaceStaircase(const glm::vec3& hitPosition) {
        StaircaseCreateInfo createInfo;
        createInfo.position = hitPosition;
        createInfo.rotation = glm::vec3(0.0f);
        World::AddStaircase(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    // Hey. Some time when you see this message, you could 
    // rewrite the generic object placement sub type thing
    // to use a string parameter "subType", that way the same
    // logic could be used for other objects with sub types,
    // like lights for example, rather than GenericObjects 
    // only as it is currently.

    void PlaceLight(const glm::vec3& hitPosition) {
        LightCreateInfo createInfo;
        createInfo.position = hitPosition;
        World::AddLight(createInfo, SpawnOffset());
        ExitObjectPlacement();
    }

    void PlaceLadder(const glm::vec3& hitPosition) {
        LadderCreateInfo createInfo;
        createInfo.position = hitPosition + glm::vec3(0.0f, 1.0f, 0.0f);
        createInfo.rotation = glm::vec3(0.0f);
        World::AddLadder(createInfo, SpawnOffset());
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