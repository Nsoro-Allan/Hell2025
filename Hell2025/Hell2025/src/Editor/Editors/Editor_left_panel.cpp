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
    EditorUI::CollapsingHeader g_outlinerHeader;
    EditorUI::StringInput g_mapNameInput;
    EditorUI::Outliner g_outliner;

    void InitLeftPanel() {
        g_mapPropertiesHeader.SetTitle("Map Editor");


        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR ||
            GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            Map* map = MapManager::GetMapByName(GetEditorMapName());
            if (map) {
                g_mapNameInput.SetLabel("Map Name");
                g_mapNameInput.SetText(GetEditorMapName());

                g_outlinerHeader.SetTitle("Outliner");

                //g_outliner.AddType("House Locations");
                //if (World::GetPickUps().size()) g_outliner.AddType("Pick Ups");
                //if (World::GetTrees().size()) g_outliner.AddType("Trees");

                std::vector<std::string> gameObjects = { "shit", "fuck" };
                std::vector<std::string> trees = {};
                g_outliner.SetItems("Game Objects", gameObjects);
                g_outliner.SetItems("Trees", trees);
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
        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            if (g_outlinerHeader.CreateImGuiElement()) {
                g_outliner.CreateImGuiElements();
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
        }
        }
    }

    void EndLeftPanel() {
        g_leftPanel.EndImGuiElement();
    }
}