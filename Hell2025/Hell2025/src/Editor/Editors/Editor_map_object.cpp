#include "Editor/Editor.h"

#include "Callbacks/Callbacks.h"
#include "Config/Config.h"
#include "Audio/Audio.h"
#include "Core/JSON.h"
#include "Editor/Gizmo.h"
#include "Managers/MapManager.h"
#include "Renderer/Renderer.h"
#include "World/SectorManager.h"
#include "World/World.h"

#include "Imgui/ImguiBackEnd.h"
#include <ImGui/imgui.h>

namespace Editor {
    namespace {
        struct ImguiElements {
            EditorUI::CollapsingHeader sectorPropertiesHeader;
            EditorUI::CollapsingHeader rendererSettingsHeader;
            EditorUI::CollapsingHeader objectSettingsHeader;
            EditorUI::DropDown heightMapDropDown;
            EditorUI::StringInput sectorNameInput;
            EditorUI::CheckBox drawGrass;
            EditorUI::CheckBox drawWater;
            EditorUI::Vec3Input objectPositon;
            EditorUI::Vec3Input objectRotation;
            EditorUI::Vec3Input objectScale;
            EditorUI::NewFileWindow newFileWindow;
            EditorUI::OpenFileWindow openFileWindow;
        } g_imguiElements;
    }

    void InitMapObjectEditorFileMenu();
    void InitMapObjectEditorPropertiesElements();
    void ReconfigureMapObjectEditorImGuiElements();

    void OpenMapObjectEditor() {
        // If it's closed, open it
        if (IsClosed()) {
            OpenEditor();
        }
        // If it's already open, do nothing
        else if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            return;
        }
        SetEditorMode(EditorMode::MAP_OBJECT_EDITOR);

        // World state
        MapInstanceCreateInfo mapInstanceCreateInfo;
        mapInstanceCreateInfo.mapName = Editor::GetEditorMapName();
        mapInstanceCreateInfo.spawnOffsetChunkX = 0;
        mapInstanceCreateInfo.spawnOffsetChunkZ = 0;
        World::ClearAllObjects();
        World::LoadMapInstancesHeightMapData({ mapInstanceCreateInfo });
        World::LoadMapInstanceObjects(Editor::GetEditorMapName(), SpawnOffset());
        World::EnableOcean();

        // Init UI
        InitFileMenuImGuiElements();
        InitLeftPanel();
        ReconfigureMapObjectEditorImGuiElements();

        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
    }

    void InitMapObjectEditor() {
        InitMapObjectEditorFileMenu();
        InitMapObjectEditorPropertiesElements();
    }

    void InitMapObjectEditorFileMenu() {
        ImguiElements& elements = g_imguiElements;

    }

    void InitMapObjectEditorPropertiesElements() {
        ImguiElements& elements = g_imguiElements;
        elements.sectorPropertiesHeader.SetTitle("Sector Properties");
        elements.rendererSettingsHeader.SetTitle("Renderer Settings");
        elements.objectSettingsHeader.SetTitle("Object Settings");
        elements.sectorNameInput.SetLabel("Name");
        elements.heightMapDropDown.SetText("Height Map");
        elements.drawGrass.SetText("Draw Grass");
        elements.drawWater.SetText("Draw Water");
        elements.objectPositon.SetText("Position");
        elements.objectRotation.SetText("Rotation");
        elements.objectScale.SetText("Scale");
        elements.newFileWindow.SetTitle("New Sector");
        elements.newFileWindow.SetCallback(nullptr);
        elements.openFileWindow.SetTitle("Open Sector");
        elements.openFileWindow.SetPath("res/sectors/");
        elements.openFileWindow.SetCallback(nullptr);
    }

    void CreateMapObjectEditorImGuiElements() {
        BeginLeftPanel();

        EndLeftPanel();
        //SectorEditorImguiElements& elements = g_sectorEditorImguiElements;
        //
        //elements.fileMenu.CreateImguiElements();
        //elements.leftPanel.BeginImGuiElement();
        //
        //const std::string& sectorName = GetSectorName();
        //SectorCreateInfo* sectorCreateInfo = SectorManager::GetSectorCreateInfoByName(sectorName);
        //
        //if (!sectorCreateInfo) return;
        //
        //// Renderer settings
        //if (elements.rendererSettingsHeader.CreateImGuiElement()) {
        //    if (elements.drawGrass.CreateImGuiElements()) {
        //        RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
        //        renderSettings.drawGrass = elements.drawGrass.GetState();
        //     
        //    }
        //    if (elements.drawWater.CreateImGuiElements()) {
        //        std::cout << elements.drawWater.GetState();
        //    }
        //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
        //}
        //
        //// Sector properties
        //if (elements.sectorPropertiesHeader.CreateImGuiElement()) {
        //    elements.sectorNameInput.CreateImGuiElement();
        //    ImGui::Dummy(ImVec2(0.0f, 20.0f));
        //}
        //
        //// Object settings
        //if (elements.objectSettingsHeader.CreateImGuiElement()) {         
        //    if (elements.objectPositon.CreateImGuiElements()) {
        //        std::cout << Util::Vec3ToString(elements.objectPositon.GetValue()) << "\n";;
        //    }
        //    if (elements.objectRotation.CreateImGuiElements()) {
        //        std::cout << Util::Vec3ToString(elements.objectRotation.GetValue()) << "\n";;
        //    }
        //    if (elements.objectScale.CreateImGuiElements()) {
        //        std::cout << Util::Vec3ToString(elements.objectScale.GetValue()) << "\n";;
        //    }
        //    ImGui::Dummy(ImVec2(0.0f, 20.0f));
        //}
        //
        //// Outliner settings
        //if (elements.outlinerHeader.CreateImGuiElement()) {
        //    elements.outliner.CreateImGuiElements();
        //    ImGui::Dummy(ImVec2(0.0f, 20.0f));
        //}
        //
        //elements.leftPanel.EndImGuiElement();
        //
        //// Windows
        //if (elements.newFileWindow.IsVisible()) {
        //    elements.newFileWindow.CreateImGuiElements();
        //}
        //if (elements.openFileWindow.IsVisible()) {
        //    elements.openFileWindow.CreateImGuiElements();
        //}
    }

    void ReconfigureMapObjectEditorImGuiElements() {

    }

    void ShowNewSectorWindow() {
        CloseAllEditorWindows();
        ImguiElements& elements = g_imguiElements;
        elements.newFileWindow.Show();
    }

    void ShowOpenSectorWindow() {
        CloseAllEditorWindows();
        ImguiElements& elements = g_imguiElements;
        elements.openFileWindow.Show();
    }

    void CloseAllMapObjectEditorWindows() {
        ImguiElements& elements = g_imguiElements;
        elements.newFileWindow.Close();
        elements.openFileWindow.Close();
    }

    void UpdateMapObjectEditor() {
        // Nothing as of yet
    }
}

