#include "Editor/Editor.h"

#include "Callbacks/Callbacks.h"
#include "Config/Config.h"
#include "Audio/Audio.h"
#include "Core/JSON.h"
#include "Editor/Gizmo.h"
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

    void OpenMapObjectEditor() {
        if (IsClosed()) {
            OpenEditor();
        }
        
        if (GetEditorMode() != EditorMode::MAP_OBJECT_EDITOR) {
            SetEditorMode(EditorMode::MAP_OBJECT_EDITOR);

            if (GetEditorMapName() != "Shit") {
                SetEditorMapName("Shit");
                World::LoadMapInstance("Shit");
            }

            InitFileMenuImGuiElements();
            InitLeftPanel();
            ReconfigureMapObjectEditorImGuiElements();

            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
        }
    }

    void UpdateMapObjectEditor() {
        // Draw perimeter
        glm::vec3 p0 = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 p1 = glm::vec3(64.0f, 0.0f, 0.0f);
        glm::vec3 p2 = glm::vec3(0.0f, 0.0f, 64.0f);
        glm::vec3 p3 = glm::vec3(64.0f, 0.0f, 64.0f);
        Renderer::DrawLine(p0, p1, GRID_COLOR, true);
        Renderer::DrawLine(p0, p2, GRID_COLOR, true);
        Renderer::DrawLine(p2, p3, GRID_COLOR, true);
        Renderer::DrawLine(p1, p3, GRID_COLOR, true);
    }
}

