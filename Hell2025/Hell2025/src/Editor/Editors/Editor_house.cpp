#include "Editor/Editor.h"

#include "Audio/Audio.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "ImGui/Types/Types.h"
#include "Input/Input.h"
#include "Managers/HouseManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include <imgui/imgui.h>

namespace Editor {
    namespace {
        struct ImguiElements {
            EditorUI::CollapsingHeader sectorPropertiesHeader;
            EditorUI::CollapsingHeader rendererSettingsHeader;
            EditorUI::CollapsingHeader objectSettingsHeader;
            EditorUI::CheckBox drawGrass;
            EditorUI::CheckBox drawWater;
            EditorUI::Vec3Input objectPositon;
            EditorUI::Vec3Input objectRotation;
            EditorUI::Vec3Input objectScale;
            EditorUI::NewFileWindow newFileWindow;
            EditorUI::OpenFileWindow openFileWindow;
        } g_imguiElements;
    }

    //void InitHouseEditorFileMenu();
    void InitHouseEditorPropertiesElements();
    //void ReconfigureHouseEditorImGuiElements();

    //struct HouseEditorEditorImguiElements {
    //    EditorUI::FileMenu fileMenu;
    //    EditorUI::LeftPanel leftPanel;
    //    EditorUI::CollapsingHeader housePropertiesHeader;
    //    EditorUI::StringInput houseNameInput;
    //    EditorUI::NewFileWindow newFileWindow;
    //    EditorUI::OpenFileWindow openFileWindow;
    //} g_houseEditorImguiElements;

    //void InitHouseEditorFileMenu();
    //void InitHouseEditorPropertiesElements();
    //void ReconfigureHMapEditorImGuiElements();

    // Wall placement
    void BeginWall();
    //void CancelWallPlacement();
    void UpdateWallPlacement();

    void OpenHouseEditor() {
        // If it's closed, open it
        if (IsClosed()) {
            OpenEditor();
        }
        // If it's already open, do nothing
        else if (GetEditorMode() == EditorMode::HOUSE_EDITOR) {
            return;
        }
        SetEditorMode(EditorMode::HOUSE_EDITOR);

        // World state
        World::LoadSingleHouse(Editor::GetEditorHouseName());
        World::DisableOcean();

        // Init UI
        InitFileMenuImGuiElements();
        InitLeftPanel();
        //ReconfigureHouseEditorImGuiElements();

        // Move player somewhere reasonable
        if (Player* player = Game::GetLocalPlayerByIndex(0)) {
            if (player->GetFootPosition().y > 10) {
                player->SetFootPosition(glm::vec3(2.25f, 0.0, 1.68f));
                player->GetCamera().SetEulerRotation(glm::vec3(-0.2f, 0.0, 0.0f));
            }
        }

        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
    }

    //void InitHouseEditor() {
    //    InitHouseEditorFileMenu();
    //    InitHouseEditorPropertiesElements();
    //}
    //
    //void InitHouseEditorFileMenu() {
    //    HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
    //
    //    EditorUI::FileMenuNode& file = elements.fileMenu.AddMenuNode("File", nullptr);
    //    file.AddChild("New", []() { ShowNewHouseWindow(); }, "F2");
    //    file.AddChild("Open", []() { ShowOpenHouseWindow(); }, "F3");
    //    file.AddChild("Save", &Callbacks::SaveHouse, "Ctrl+S");
    //    file.AddChild("Revert", nullptr);
    //    file.AddChild("Delete", nullptr);
    //    file.AddChild("Duplicate", nullptr);
    //    file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");
    //
    //    EditorUI::FileMenuNode& editor = elements.fileMenu.AddMenuNode("Editor");
    //    editor.AddChild("House", &Callbacks::OpenHouseEditor, "F4");
    //    editor.AddChild("Map Objects", &Callbacks::OpenMapObjectEditor, "F5");
    //    editor.AddChild("Map Height", &Callbacks::OpenMapHeightEditor, "F6");
    //
    //    EditorUI::FileMenuNode& insert = elements.fileMenu.AddMenuNode("Insert");
    //    insert.AddChild("Door", &Callbacks::BeginAddingDoor, "");
    //    insert.AddChild("Picture Frame", &Callbacks::BeginAddingPictureFrame, "");
    //    insert.AddChild("Wall", &Callbacks::BeginAddingWall, "");
    //    insert.AddChild("Window", &Callbacks::BeginAddingWindow, "");
    //}
    //
    //void InitHouseEditorPropertiesElements() {
    //    HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
    //    //elements.rendererSettingsHeader.SetTitle("Renderer Settings");
    //    //elements.drawGrass.SetText("Draw Grass");
    //    //elements.drawWater.SetText("Draw Water");
    //    elements.housePropertiesHeader.SetTitle("House Properties");
    //    elements.houseNameInput.SetLabel("House name");
    //    //elements.test.SetText("Integer Test");
    //    //elements.test.SetRange(-1, 10);
    //    //elements.test.SetValue(8);
    //    //elements.test2.SetText("Slider Test");
    //    //elements.test2.SetRange(-1.0f, 10.0f);
    //    //elements.test2.SetValue(8.0f);
    //
    //    elements.newFileWindow.SetTitle("New House");
    //    elements.newFileWindow.SetCallback(Callbacks::NewHouse);
    //    elements.openFileWindow.SetTitle("Open House");
    //    elements.openFileWindow.SetPath("res/houses/");
    //    elements.openFileWindow.SetCallback(Callbacks::OpenHouse);
    //}
    //
    //void ReconfigureHouseEditorImGuiElements() {
    //    HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
    //
    //    // Update name input with height map name
    //    elements.houseNameInput.SetText(World::GetCurrentMapName());
    //
    //    //RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
    //    //elements.drawGrass.SetState(renderSettings.drawGrass);
    //}
    //
    //void CreateHouseEditorImGuiElements() {
    //    HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
    //    elements.fileMenu.CreateImguiElements();
    //    elements.leftPanel.BeginImGuiElement();
    //
    //    // Renderer settings
    //    //if (elements.rendererSettingsHeader.CreateImGuiElement()) {
    //    //    if (elements.drawGrass.CreateImGuiElements()) {
    //    //        RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
    //    //        renderSettings.drawGrass = elements.drawGrass.GetState();
    //    //    }
    //    //    if (elements.drawWater.CreateImGuiElements()) {
    //    //        std::cout << elements.drawWater.GetState();
    //    //    }
    //    //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    //    //}
    //
    //    // Height map properties
    //    if (elements.housePropertiesHeader.CreateImGuiElement()) {
    //        elements.houseNameInput.CreateImGuiElement();
    //        //elements.test.CreateImGuiElements();
    //        //elements.test2.CreateImGuiElements();
    //
    //    }
    //
    //    elements.leftPanel.EndImGuiElement();
    //
    //    // Windows
    //    if (elements.newFileWindow.IsVisible()) {
    //        elements.newFileWindow.CreateImGuiElements();
    //    }
    //    if (elements.openFileWindow.IsVisible()) {
    //        elements.openFileWindow.CreateImGuiElements();
    //    }
    //}

    void InitHouseEditor() {
        InitHouseEditorPropertiesElements();
    }

    void CreateHouseEditorImGuiElements() {
        BeginLeftPanel();

        EndLeftPanel();
    }


    void InitHouseEditorPropertiesElements() {

    }

    void UpdateHouseEditor() {
        // Restrict renderer states
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();
        while (rendererSettings.rendererOverrideState != RendererOverrideState::NONE &&
               rendererSettings.rendererOverrideState != RendererOverrideState::CAMERA_NDOTL) {
            Renderer::NextRendererOverrideState();
        }


        // Test mouse hover on point
        //glm::vec3 testPoint = glm::vec3(0, 1, 0);
        //glm::vec3 color = WHITE;
        //Viewport* viewport = ViewportManager::GetViewportByIndex(0);
        //SpaceCoords gbufferSpaceCoords = viewport->GetGBufferSpaceCoords();
        //int mouseX = gbufferSpaceCoords.localMouseX;
        //int mouseY = gbufferSpaceCoords.localMouseY;
        //int screenWidth = gbufferSpaceCoords.width;
        //int screenHeight = gbufferSpaceCoords.height;
        //glm::mat4 projectionView = RenderDataManager::GetViewportData()[0].projectionView;
        //glm::ivec2 testPosScreenSpace = Util::WorldToScreenCoords(testPoint, projectionView, screenWidth, screenHeight, true);
        //glm::ivec2 mousePos = glm::ivec2(mouseX, mouseY);
        //int threshold = 20;
        //if (Util::IsWithinThreshold(mousePos, testPosScreenSpace, threshold)) {
        //    color = OUTLINE_COLOR;
        //}
        //Renderer::DrawPoint(testPoint, color);

        



        // Render selected wall/plane lines and vertices
       //if (GetSelectedObjectType() == ObjectType::WALL) {
       //   // MOVED TO EDITOR_OBJECTS.cpp
       //   // MOVED TO EDITOR_OBJECTS.cpp
       //   // MOVED TO EDITOR_OBJECTS.cpp
       //   // MOVED TO EDITOR_OBJECTS.cpp
       //   // MOVED TO EDITOR_OBJECTS.cpp
       //}
       //if (GetSelectedObjectType() == ObjectType::PLANE) {
       //    Plane* plane = World::GetPlaneByObjectId(GetSelectedObjectId());
       //    if (plane) {
       //        plane->DrawEdges(OUTLINE_COLOR);
       //        plane->DrawVertices(OUTLINE_COLOR);
       //    }
       //}
    }

    void ShowNewHouseWindow() {
        CloseAllEditorWindows();
    }

    void ShowOpenHouseWindow() {
        CloseAllEditorWindows();
    }

    void CloseAllHouseEditorWindows() {

    }
}