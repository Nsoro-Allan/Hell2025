#include "Editor/Editor.h"
#include "AssetManagement/AssetManager.h"
#include "HellLogging.h"
#include "Imgui/ImguiBackEnd.h"
#include <ImGui/imgui.h>
#include "Imgui/Types/Types.h"
#include "Managers/MapManager.h"
#include "World/World.h"

#include <Util.h>

namespace Editor {
    EditorUI::LeftPanel g_leftPanel;

    EditorUI::CollapsingHeader g_settingsHeader;
    EditorUI::CheckBox g_backfaceCulling;

    EditorUI::CollapsingHeader g_mapPropertiesHeader;
    EditorUI::CollapsingHeader g_objectPropertiesHeader;
    EditorUI::CollapsingHeader g_outlinerHeader;
    EditorUI::StringInput g_mapNameInput;
    EditorUI::StringInput g_objectNameInput;
    EditorUI::FloatInput g_positionX;
    EditorUI::FloatInput g_positionY;
    EditorUI::FloatInput g_positionZ;
    EditorUI::FloatSliderInput g_rotationX;
    EditorUI::FloatSliderInput g_rotationY;
    EditorUI::FloatSliderInput g_rotationZ;
    EditorUI::Outliner g_outliner;
    EditorUI::DropDown g_materialDropDown;

    EditorUI::FloatInput g_textureScale;
    EditorUI::FloatSliderInput g_textureOffsetU;
    EditorUI::FloatSliderInput g_textureOffsetV;

    EditorUI::FloatInput g_heightFloatInput;

    EditorUI::FloatInput g_housePlaneP0X;
    EditorUI::FloatInput g_housePlaneP0Y;
    EditorUI::FloatInput g_housePlaneP0Z;
    EditorUI::FloatInput g_housePlaneP1X;
    EditorUI::FloatInput g_housePlaneP1Y;
    EditorUI::FloatInput g_housePlaneP1Z;
    EditorUI::FloatInput g_housePlaneP2X;
    EditorUI::FloatInput g_housePlaneP2Y;
    EditorUI::FloatInput g_housePlaneP2Z;
    EditorUI::FloatInput g_housePlaneP3X;
    EditorUI::FloatInput g_housePlaneP3Y;
    EditorUI::FloatInput g_housePlaneP3Z;

    // Door stuff
    EditorUI::DropDown g_doorType;
    EditorUI::DropDown g_doorFrontMaterial;
    EditorUI::DropDown g_doorBackMaterial;    
    EditorUI::DropDown g_doorFrameFrontMaterial;
    EditorUI::DropDown g_doorFrameBackMaterial;
    EditorUI::CheckBox g_doorHasDeadLock;
    EditorUI::CheckBox g_doorDeadLockedAtStart;

    // Pickup stuff
    EditorUI::CheckBox g_pickUpDisablePhysicsAtSpawn;
    EditorUI::CheckBox g_pickUpRespawn;

    void InitLeftPanel() {
        g_mapPropertiesHeader.SetTitle("Map Editor");
        g_objectPropertiesHeader.SetTitle("Properties");

        g_settingsHeader.SetTitle("Settings");
        g_backfaceCulling.SetText("Backface culling");
        g_backfaceCulling.SetState(BackfaceCullingEnabled());

        g_materialDropDown.SetText("Material");
        g_materialDropDown.SetOptions(AssetManager::GetMaterialNames());

        g_heightFloatInput.SetText("Height");
        g_heightFloatInput.SetRange(0.1f, 100.0f);

        g_textureScale.SetText("Tex Scale");
        g_textureScale.SetRange(0.00f, 100.0f);
        g_textureOffsetU.SetText("Tex Offset U");
        g_textureOffsetU.SetRange(-1.0f, 1.0f);
        g_textureOffsetV.SetText("Tex Offset V");
        g_textureOffsetV.SetRange(-1.0f, 1.0f);

        g_housePlaneP0X.SetText("P0 X");
        g_housePlaneP0Y.SetText("P0 Y");
        g_housePlaneP0Z.SetText("P0 Z");
        g_housePlaneP1X.SetText("P1 X");
        g_housePlaneP1Y.SetText("P1 Y");
        g_housePlaneP1Z.SetText("P1 Z");
        g_housePlaneP2X.SetText("P2 X");
        g_housePlaneP2Y.SetText("P2 Y");
        g_housePlaneP2Z.SetText("P2 Z");
        g_housePlaneP3X.SetText("P3 X");
        g_housePlaneP3Y.SetText("P3 Y");
        g_housePlaneP3Z.SetText("P3 Z");

        g_doorType.SetText("Type");
        g_doorFrontMaterial.SetText("Front Material");
        g_doorBackMaterial.SetText("Back Material");
        g_doorFrameFrontMaterial.SetText("Frame Front Material");
        g_doorFrameBackMaterial.SetText("Frame Back Material");
        g_doorHasDeadLock.SetText("Has Deadlock");
        g_doorDeadLockedAtStart.SetText("Deadlocked at start");

        g_pickUpDisablePhysicsAtSpawn.SetText("No PhysX at Start");
        g_pickUpRespawn.SetText("Respawn");
    }

    void UpdateOutliner() {
        if (GetEditorMode() == EditorMode::HOUSE_EDITOR ||
            GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            Map* map = MapManager::GetMapByName(GetEditorMapName());
            if (map) {
                g_mapNameInput.SetLabel("Map Name");
                g_mapNameInput.SetText(GetEditorMapName());

                g_outlinerHeader.SetTitle("Outliner");

                g_outliner.SetItems("Ceilings", GetCeilingNames());
                g_outliner.SetItems("Doors", GetDoorNames());
                g_outliner.SetItems("Floors", GetFloorNames());
                g_outliner.SetItems("Generic Objects", GetGenericObjectNames());
                g_outliner.SetItems("House Planes", GetUndefinedHousePlaneNames());
                g_outliner.SetItems("Trees", GetTreeNames());
                g_outliner.SetItems("Walls", GetWallNames());

                g_objectNameInput.SetLabel("Name");

                g_positionX.SetText("Position X");
                g_positionY.SetText("Position Y");
                g_positionZ.SetText("Position Z");

                g_rotationX.SetText("Rotation X");
                g_rotationY.SetText("Rotation Y");
                g_rotationZ.SetText("Rotation Z");
                g_rotationX.SetRange(-HELL_PI, HELL_PI);
                g_rotationY.SetRange(-HELL_PI, HELL_PI);
                g_rotationZ.SetRange(-HELL_PI, HELL_PI);
            }
        }

        // move to UpdateObjectProperties()
        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR || GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            if (GetSelectedObjectType() == ObjectType::TREE) {
                Tree* tree = World::GetTreeByObjectId(GetSelectedObjectId());
                if (tree) {
                    g_objectNameInput.SetText(tree->GetEditorName());
                    g_positionX.SetValue(tree->GetPosition().x);
                    g_positionY.SetValue(tree->GetPosition().y);
                    g_positionZ.SetValue(tree->GetPosition().z);
                    g_rotationX.SetValue(tree->GetRotation().x);
                    g_rotationY.SetValue(tree->GetRotation().y);
                    g_rotationZ.SetValue(tree->GetRotation().z);
                }
            }
        }
    }

    void BeginLeftPanel() {
        bool houseMeshUpdateRequired = false;

        g_leftPanel.BeginImGuiElement();

        // Settings
        if (g_settingsHeader.CreateImGuiElement()) {
            if (g_backfaceCulling.CreateImGuiElements()) {
                SetBackfaceCulling(g_backfaceCulling.GetState());
            }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
        }

        // Map properties
        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR ||
            GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            if (g_mapPropertiesHeader.CreateImGuiElement()) {
                g_mapNameInput.CreateImGuiElement();
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
        }

        // Outliner
        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR ||
            GetEditorMode() == EditorMode::HOUSE_EDITOR) {
            if (g_outlinerHeader.CreateImGuiElement()) {
                float outlinerHeight = BackEnd::GetCurrentWindowHeight() * 0.45f;
                g_outliner.CreateImGuiElements(outlinerHeight);
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
            }
        }

        // Object properties
        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR || GetEditorMode() == EditorMode::HOUSE_EDITOR) {
            if (g_objectPropertiesHeader.CreateImGuiElement()) {
                if (GetSelectedObjectType() != ObjectType::NO_TYPE) {
                    g_objectNameInput.CreateImGuiElement();
                }

                // Trees (LIKELY BROKEN)
                if (GetSelectedObjectType() == ObjectType::TREE) {
                    Tree* tree = World::GetTreeByObjectId(GetSelectedObjectId());
                    if (tree) {
                        if (g_positionX.CreateImGuiElements())  tree->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                        if (g_positionY.CreateImGuiElements())  tree->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                        if (g_positionZ.CreateImGuiElements())  tree->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));

                        if (g_rotationX.CreateImGuiElements())  tree->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                        if (g_rotationY.CreateImGuiElements())  tree->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                        if (g_rotationZ.CreateImGuiElements())  tree->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                    }
                }

                // Doors (BARELY FUNCITONAL)
                if (Door* door = World::GetDoorByObjectId(GetSelectedObjectId())) {
                    g_positionX.SetValue(door->GetPosition().x);
                    g_positionY.SetValue(door->GetPosition().y);
                    g_positionZ.SetValue(door->GetPosition().z);
                    g_rotationY.SetValue(door->GetRotation().y);

                    std::vector<std::string> types;
                    types.push_back(Util::DoorTypeToString(DoorType::STANDARD_A));
                    types.push_back(Util::DoorTypeToString(DoorType::STANDARD_B));
                    types.push_back(Util::DoorTypeToString(DoorType::STAINED_GLASS));
                                          
                    std::vector<std::string> materialTypes;
                    materialTypes.push_back(Util::DoorMaterialTypeToString(DoorMaterialType::RESIDENT_EVIL));
                    materialTypes.push_back(Util::DoorMaterialTypeToString(DoorMaterialType::WHITE_PAINT));

                    g_doorType.SetOptions(types);
                    g_doorFrontMaterial.SetOptions(materialTypes);
                    g_doorBackMaterial.SetOptions(materialTypes);
                    g_doorFrameFrontMaterial.SetOptions(materialTypes);
                    g_doorFrameBackMaterial.SetOptions(materialTypes);

                    g_doorType.SetCurrentOption(Util::DoorTypeToString(door->GetType()));
                    g_doorFrontMaterial.SetCurrentOption(Util::DoorMaterialTypeToString(door->GetMaterialTypeFront()));
                    g_doorBackMaterial.SetCurrentOption(Util::DoorMaterialTypeToString(door->GetMaterialTypeBack()));
                    g_doorFrameFrontMaterial.SetCurrentOption(Util::DoorMaterialTypeToString(door->GetMaterialTypeFrameFront()));
                    g_doorFrameBackMaterial.SetCurrentOption(Util::DoorMaterialTypeToString(door->GetMaterialTypeFrameBack()));

                    g_doorHasDeadLock.SetState(door->GetDeadLockState());
                    g_doorDeadLockedAtStart.SetState(door->GetDeadLockedAtInitState());

                    if (g_positionX.CreateImGuiElements())  door->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                    if (g_positionY.CreateImGuiElements())  door->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                    if (g_positionZ.CreateImGuiElements())  door->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                    if (g_rotationY.CreateImGuiElements()) {}; // TODO

                    if (g_doorType.CreateImGuiElements()) {
                        door->SetType(Util::StringToDoorType(g_doorType.GetSelectedOptionText()));
                    }
                    if (g_doorFrontMaterial.CreateImGuiElements()) {
                        door->SetFrontMaterial(Util::StringToDoorMaterialType(g_doorFrontMaterial.GetSelectedOptionText()));
                    }
                    if (g_doorBackMaterial.CreateImGuiElements()) {
                        door->SetBackMaterial(Util::StringToDoorMaterialType(g_doorBackMaterial.GetSelectedOptionText()));
                    }
                    if (g_doorFrameFrontMaterial.CreateImGuiElements()) {
                        door->SetFrameFrontMaterial(Util::StringToDoorMaterialType(g_doorFrameFrontMaterial.GetSelectedOptionText()));
                    }
                    if (g_doorFrameBackMaterial.CreateImGuiElements()) {
                        door->SetFrameBackMaterial(Util::StringToDoorMaterialType(g_doorFrameBackMaterial.GetSelectedOptionText()));
                    }
                    if (g_doorHasDeadLock.CreateImGuiElements()) {
                        door->SetDeadLockState(g_doorHasDeadLock.GetState());
                    }
                    if (g_doorDeadLockedAtStart.CreateImGuiElements()) {
                        door->SetDeadLockedAtInitState(g_doorDeadLockedAtStart.GetState());
                    }
                }


                // Pick Ups
                if (GetSelectedObjectType() == ObjectType::PICK_UP) {
                    if (PickUp* pickUp = World::GetPickUpByObjectId(GetSelectedObjectId())) {
                        // Retrieve state
                        g_positionX.SetValue(pickUp->GetPosition().x);
                        g_positionY.SetValue(pickUp->GetPosition().y);
                        g_positionZ.SetValue(pickUp->GetPosition().z);
                        g_rotationX.SetValue(pickUp->GetRotation().x);
                        g_rotationY.SetValue(pickUp->GetRotation().y);
                        g_rotationZ.SetValue(pickUp->GetRotation().y);
                        g_pickUpDisablePhysicsAtSpawn.SetState(pickUp->GetDisabledPhysicsAtSpawnState());
                        g_pickUpRespawn.SetState(pickUp->GetRespawnState());

                        // Render and set state
                        if (g_positionX.CreateImGuiElements())                      pickUp->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                        if (g_positionY.CreateImGuiElements())                      pickUp->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                        if (g_positionZ.CreateImGuiElements())                      pickUp->SetPosition(glm::vec3(g_positionX.GetValue(), g_positionY.GetValue(), g_positionZ.GetValue()));
                        if (g_rotationX.CreateImGuiElements())                      pickUp->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                        if (g_rotationY.CreateImGuiElements())                      pickUp->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                        if (g_rotationZ.CreateImGuiElements())                      pickUp->SetRotation(glm::vec3(g_rotationX.GetValue(), g_rotationY.GetValue(), g_rotationZ.GetValue()));
                        if (g_pickUpDisablePhysicsAtSpawn.CreateImGuiElements())    pickUp->SetDisabledPhysicsAtSpawnState(g_pickUpDisablePhysicsAtSpawn.GetState());
                        if (g_pickUpRespawn.CreateImGuiElements())                  pickUp->SetRespawnState(g_pickUpRespawn.GetState());
                    }
                }


                // House planes (aka floors and ceilings)
                if (HousePlane* housePlane = World::GetHousePlaneByObjectId(GetSelectedObjectId())) {
                    bool housePlaneUpdated = false;

                    g_materialDropDown.SetCurrentOption(housePlane->GetCreateInfo().materialName);
                    g_housePlaneP0X.SetValue(housePlane->GetCreateInfo().p0.x);
                    g_housePlaneP0Y.SetValue(housePlane->GetCreateInfo().p0.y);
                    g_housePlaneP0Z.SetValue(housePlane->GetCreateInfo().p0.z);
                    g_housePlaneP1X.SetValue(housePlane->GetCreateInfo().p1.x);
                    g_housePlaneP1Y.SetValue(housePlane->GetCreateInfo().p1.y);
                    g_housePlaneP1Z.SetValue(housePlane->GetCreateInfo().p1.z);
                    g_housePlaneP2X.SetValue(housePlane->GetCreateInfo().p2.x);
                    g_housePlaneP2Y.SetValue(housePlane->GetCreateInfo().p2.y);
                    g_housePlaneP2Z.SetValue(housePlane->GetCreateInfo().p2.z);
                    g_housePlaneP3X.SetValue(housePlane->GetCreateInfo().p3.x);
                    g_housePlaneP3Y.SetValue(housePlane->GetCreateInfo().p3.y);
                    g_housePlaneP3Z.SetValue(housePlane->GetCreateInfo().p3.z);
                    g_textureOffsetU.SetValue(housePlane->GetCreateInfo().textureOffsetU);
                    g_textureOffsetV.SetValue(housePlane->GetCreateInfo().textureOffsetV);
                    g_textureScale.SetValue(housePlane->GetCreateInfo().textureScale);

                    if (g_materialDropDown.CreateImGuiElements()) {
                        housePlane->SetMaterial(g_materialDropDown.GetSelectedOptionText());
                        houseMeshUpdateRequired = true;
                        housePlaneUpdated = true;
                    }

                    if (g_housePlaneP0X.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP0Y.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP0Z.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP1X.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP1Y.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP1Z.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP2X.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP2Y.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP2Z.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP3X.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP3Y.CreateImGuiElements()) housePlaneUpdated = true;
                    if (g_housePlaneP3Z.CreateImGuiElements()) housePlaneUpdated = true;

                    if (g_textureScale.CreateImGuiElements()) {
                        housePlane->SetTextureScale(g_textureScale.GetValue());
                        houseMeshUpdateRequired = true;
                    }
                    if (g_textureOffsetU.CreateImGuiElements()) {
                        housePlane->SetTextureOffsetU(g_textureOffsetU.GetValue());
                        houseMeshUpdateRequired = true;
                    }
                    if (g_textureOffsetV.CreateImGuiElements()) {
                        housePlane->SetTextureOffsetV(g_textureOffsetV.GetValue());
                        houseMeshUpdateRequired = true;
                    }

                    if (housePlaneUpdated) {
                        HousePlaneCreateInfo& createInfo = housePlane->GetCreateInfo();
                        createInfo.p0.x = g_housePlaneP0X.GetValue();
                        createInfo.p0.y = g_housePlaneP0Y.GetValue();
                        createInfo.p0.z = g_housePlaneP0Z.GetValue();
                        createInfo.p1.x = g_housePlaneP1X.GetValue();
                        createInfo.p1.y = g_housePlaneP1Y.GetValue();
                        createInfo.p1.z = g_housePlaneP1Z.GetValue();
                        createInfo.p2.x = g_housePlaneP2X.GetValue();
                        createInfo.p2.y = g_housePlaneP2Y.GetValue();
                        createInfo.p2.z = g_housePlaneP2Z.GetValue();
                        createInfo.p3.x = g_housePlaneP3X.GetValue();
                        createInfo.p3.y = g_housePlaneP3Y.GetValue();
                        createInfo.p3.z = g_housePlaneP3Z.GetValue();
                        housePlane->UpdateVertexDataFromCreateInfo();
                        houseMeshUpdateRequired = true;
                    }
                }

                // Walls
                if (Wall* wall = World::GetWallByObjectId(GetSelectedObjectId())) {
                    g_materialDropDown.SetCurrentOption(wall->GetCreateInfo().materialName);
                    g_heightFloatInput.SetValue(wall->GetCreateInfo().height);
                    g_textureOffsetU.SetValue(wall->GetCreateInfo().textureOffsetU);
                    g_textureOffsetV.SetValue(wall->GetCreateInfo().textureOffsetV);
                    g_textureScale.SetValue(wall->GetCreateInfo().textureScale);

                    // Material
                    if (g_materialDropDown.CreateImGuiElements()) {
                        wall->SetMaterial(g_materialDropDown.GetSelectedOptionText());
                        houseMeshUpdateRequired = true;
                    }

                    // Height
                    if (g_heightFloatInput.CreateImGuiElements()) {
                        wall->SetHeight(g_heightFloatInput.GetValue());
                        houseMeshUpdateRequired = true;
                    }

                    // Texture settings
                    if (g_textureScale.CreateImGuiElements()) {
                        wall->SetTextureScale(g_textureScale.GetValue());
                        houseMeshUpdateRequired = true;
                    }
                    if (g_textureOffsetU.CreateImGuiElements()) {
                        wall->SetTextureOffsetU(g_textureOffsetU.GetValue());
                        houseMeshUpdateRequired = true;
                    }
                    if (g_textureOffsetV.CreateImGuiElements()) {
                        wall->SetTextureOffsetV(g_textureOffsetV.GetValue());
                        houseMeshUpdateRequired = true;
                    }
                }

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
            }
        }

        if (houseMeshUpdateRequired) {
            World::UpdateHouseMeshBuffer();
        }
    }

    void EndLeftPanel() {
        g_leftPanel.EndImGuiElement();
    }
}