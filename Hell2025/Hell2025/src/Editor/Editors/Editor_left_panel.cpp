#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Imgui/ImguiBackEnd.h"
#include <ImGui/imgui.h>
#include "Imgui/Types/Types.h"
#include "Managers/MapManager.h"
#include "World/World.h"

#include <Util.h>

namespace Editor {
    EditorUI::LeftPanel g_leftPanel;

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

    void InitLeftPanel() {
        g_mapPropertiesHeader.SetTitle("Map Editor");
        g_objectPropertiesHeader.SetTitle("Properties");

        UpdateOutliner();
    }

    void UpdateOutliner() {
        if (GetEditorMode() == EditorMode::HOUSE_EDITOR ||
            GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            Map* map = MapManager::GetMapByName(GetEditorMapName());
            if (map) {
                g_mapNameInput.SetLabel("Map Name");
                g_mapNameInput.SetText(GetEditorMapName());

                g_outlinerHeader.SetTitle("Outliner");

                std::vector<std::string> gameObjects = { "shit", "fuck" };

                g_outliner.SetItems("Game Objects", gameObjects);
                g_outliner.SetItems("Trees", GetTreeNames());

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
        g_leftPanel.BeginImGuiElement();

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
        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            if (g_objectPropertiesHeader.CreateImGuiElement()) {
                if (GetSelectedObjectType() != ObjectType::NO_TYPE) {
                    g_objectNameInput.CreateImGuiElement();
                }

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

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
            }
        }
        
    }

    void EndLeftPanel() {
        g_leftPanel.EndImGuiElement();
    }
}