#include "Editor/Editor.h"

#include "Audio/Audio.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "ImGui/Types/Types.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/HouseManager.h"
#include "World/World.h"
#include <imgui/imgui.h>

namespace Editor {

    struct HouseEditorEditorImguiElements {
        EditorUI::FileMenu fileMenu;
        EditorUI::LeftPanel leftPanel;
        EditorUI::CollapsingHeader housePropertiesHeader;
        EditorUI::StringInput houseNameInput;
        EditorUI::NewFileWindow newFileWindow;
        EditorUI::OpenFileWindow openFileWindow;
    } g_houseEditorImguiElements;

    std::string g_currentFilename = "";

    void InitHouseEditorFileMenu();
    void InitHouseEditorPropertiesElements();
    void ReconfigureHMapEditorImGuiElements();

    // Wall placement
    void BeginWall();
    void CancelWallPlacement();
    void UpdateWallPlacement();

    void InitHouseEditor() {
        InitHouseEditorFileMenu();
        InitHouseEditorPropertiesElements();
    }

    void InitHouseEditorFileMenu() {
        HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;

        EditorUI::FileMenuNode& file = elements.fileMenu.AddMenuNode("File", nullptr);
        file.AddChild("New", []() { ShowNewHouseWindow(); }, "F2");
        file.AddChild("Open", []() { ShowOpenHouseWindow(); }, "F3");
        file.AddChild("Save", &Callbacks::SaveHouse, "Ctrl+S");
        file.AddChild("Revert", nullptr);
        file.AddChild("Delete", nullptr);
        file.AddChild("Duplicate", nullptr);
        file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");

        EditorUI::FileMenuNode& editor = elements.fileMenu.AddMenuNode("Editor");
        editor.AddChild("House", &Callbacks::OpenHouseEditor, "F4");
        editor.AddChild("Map Objects", &Callbacks::OpenMapObjectEditor, "F5");
        editor.AddChild("Map Height", &Callbacks::OpenMapHeightEditor, "F6");

        EditorUI::FileMenuNode& insert = elements.fileMenu.AddMenuNode("Insert");
        insert.AddChild("Door", &Callbacks::BeginAddingDoor, "");
        insert.AddChild("Picture Frame", &Callbacks::BeginAddingPictureFrame, "");
        insert.AddChild("Wall", &Callbacks::BeginAddingWall, "");
        insert.AddChild("Window", &Callbacks::BeginAddingWindow, "");
    }

    void InitHouseEditorPropertiesElements() {
        HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
        //elements.rendererSettingsHeader.SetTitle("Renderer Settings");
        //elements.drawGrass.SetText("Draw Grass");
        //elements.drawWater.SetText("Draw Water");
        elements.housePropertiesHeader.SetTitle("House Properties");
        elements.houseNameInput.SetLabel("House name");
        //elements.test.SetText("Integer Test");
        //elements.test.SetRange(-1, 10);
        //elements.test.SetValue(8);
        //elements.test2.SetText("Slider Test");
        //elements.test2.SetRange(-1.0f, 10.0f);
        //elements.test2.SetValue(8.0f);

        elements.newFileWindow.SetTitle("New House");
        elements.newFileWindow.SetCallback(Callbacks::NewHouse);
        elements.openFileWindow.SetTitle("Open House");
        elements.openFileWindow.SetPath("res/houses/");
        elements.openFileWindow.SetCallback(Callbacks::OpenHouse);
    }

    void ReconfigureHouseEditorImGuiElements() {
        HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;

        // Update name input with height map name
        elements.houseNameInput.SetText(World::GetCurrentMapName());

        //RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
        //elements.drawGrass.SetState(renderSettings.drawGrass);
    }

    void CreateHouseEditorImGuiElements() {
        HouseEditorEditorImguiElements& elements = g_houseEditorImguiElements;
        elements.fileMenu.CreateImguiElements();
        elements.leftPanel.BeginImGuiElement();

        // Renderer settings
        //if (elements.rendererSettingsHeader.CreateImGuiElement()) {
        //    if (elements.drawGrass.CreateImGuiElements()) {
        //        RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
        //        renderSettings.drawGrass = elements.drawGrass.GetState();
        //    }
        //    if (elements.drawWater.CreateImGuiElements()) {
        //        std::cout << elements.drawWater.GetState();
        //    }
        //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
        //}

        // Height map properties
        if (elements.housePropertiesHeader.CreateImGuiElement()) {
            elements.houseNameInput.CreateImGuiElement();
            //elements.test.CreateImGuiElements();
            //elements.test2.CreateImGuiElements();

        }

        elements.leftPanel.EndImGuiElement();

        // Windows
        if (elements.newFileWindow.IsVisible()) {
            elements.newFileWindow.CreateImGuiElements();
        }
        if (elements.openFileWindow.IsVisible()) {
            elements.openFileWindow.CreateImGuiElements();
        }
    }

    void OpenHouseEditor() {
        Audio::PlayAudio(AUDIO_SELECT, 1.0f);
        World::DisableOcean();

        if (Editor::GetEditorMode() != EditorMode::HOUSE_EDITOR) {

            g_currentFilename = "TestHouse";
            HouseCreateInfo* houseCreateInfo = HouseManager::GetHouseCreateInfoByFilename(g_currentFilename);
            World::LoadSingleHouse(houseCreateInfo);

            // Move player somewhere reasonable
            Player* player = Game::GetLocalPlayerByIndex(0);
            if (player) {
                player->SetFootPosition(glm::vec3(2.25f, 0.0, 1.68f));
                player->GetCamera().SetEulerRotation(glm::vec3(-0.2f, 0.0, 0.0f));
            }

            Editor::SetEditorMode(EditorMode::HOUSE_EDITOR);
            if (Editor::IsClosed()) {
                Editor::OpenEditor();
            }
        }
    }

    void UpdateHouseEditor() {
        // Restrict renderer states
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();
        while (rendererSettings.rendererOverrideState != RendererOverrideState::NONE &&
               rendererSettings.rendererOverrideState != RendererOverrideState::CAMERA_NDOTL) {
            Renderer::NextRendererOverrideState();
        }
        // Draw Grid
        float gridWorldSpaceSize = 5.0f;
        float gridSpacing = 0.5f;
        float yHeight = -0.01f;

        for (float x = -gridWorldSpaceSize; x <= gridWorldSpaceSize; x += gridSpacing) {
            for (float z = -gridWorldSpaceSize; z <= gridWorldSpaceSize; z += gridSpacing) {
                glm::vec3 p1 = glm::vec3(x, yHeight, -gridWorldSpaceSize);
                glm::vec3 p2 = glm::vec3(x, yHeight, gridWorldSpaceSize);
                glm::vec3 p3 = glm::vec3(-gridWorldSpaceSize, yHeight, z);
                glm::vec3 p4 = glm::vec3(gridWorldSpaceSize, yHeight, z);
                Renderer::DrawLine(p1, p2, GRID_COLOR, true);
                Renderer::DrawLine(p3, p4, GRID_COLOR, true);
            }
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

        // Draw Grid
        const Resolutions& resolutions = Config::GetResolutions();
        float pixelSizeX = 2.0f / resolutions.gBuffer.x;
        float pixelSizeY = 2.0f / resolutions.gBuffer.y;

        for (int x = 0; x <= 1; x++) {
            for (int y = 0; y <= 1; y++) {
                glm::vec3 n = glm::vec3(gridWorldSpaceSize, yHeight, 0.0f);
                glm::vec3 s = glm::vec3(-gridWorldSpaceSize, yHeight, 0.0f);
                glm::vec3 e = glm::vec3(0.0f, yHeight, gridWorldSpaceSize);
                glm::vec3 w = glm::vec3(0.0f, yHeight, -gridWorldSpaceSize);

                Renderer::DrawLine(n, s, WHITE, true);
                Renderer::DrawLine(e, w, WHITE, true);
            }
        }



        // Render selected wall/plane lines and vertices
        if (GetSelectedObjectType() == ObjectType::WALL) {
            Wall* wall = World::GetWallByObjectId(GetSelectedObjectId());
            if (wall) {
                wall->DrawSegmentVertices(OUTLINE_COLOR);
                wall->DrawSegmentLines(OUTLINE_COLOR);
            }
        }
        if (GetSelectedObjectType() == ObjectType::PLANE) {
            Plane* plane = World::GetPlaneByObjectId(GetSelectedObjectId());
            if (plane) {
                plane->DrawEdges(OUTLINE_COLOR);
                plane->DrawVertices(OUTLINE_COLOR);
            }
        }
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