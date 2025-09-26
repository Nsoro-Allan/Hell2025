#include "Editor/Editor.h"

#include "Audio/Audio.h"
#include "BackEnd/BackEnd.h"
#include "Core/JSON.h"
#include "Editor/Gizmo.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "World/SectorManager.h"
#include "World/World.h"
#include "Viewport/ViewportManager.h"

#include "Imgui/ImguiBackEnd.h"
#include <ImGui/imgui.h>

#include "HellLogging.h"

namespace Editor {
    namespace {
        struct ImguiElements {
            //EditorUI::LeftPanel leftPanel;
            EditorUI::CollapsingHeader rendererSettingsHeader;
            EditorUI::CollapsingHeader mapHeightPropertiesHeader;
            EditorUI::StringInput mapHeightNameInput;
            EditorUI::CheckBox drawGrass;
            EditorUI::CheckBox drawWater;
            EditorUI::NewFileWindow newFileWindow;
            EditorUI::OpenFileWindow openFileWindow;
            EditorUI::FloatSliderInput brushSize;
            EditorUI::FloatSliderInput brushStrength;
            EditorUI::FloatSliderInput noiseStrength;
            EditorUI::FloatSliderInput noiseScale;
            EditorUI::FloatInput minPaintHeight;
            EditorUI::FloatInput maxPaintHeight;
            EditorUI::IntegerInput chunkWidth;
            EditorUI::IntegerInput chunkDepth;
        } g_imguiElements;
    }

    float g_brushSize = 16;
    float g_brushStrength = 1;
    float g_noiseStrength = 1;
    float g_minPaintHeight = 0.0f;
    float g_noiseScale = 0.5f;
    float g_maxPaintHeight = HEIGHTMAP_SCALE_Y;

    void InitMapHeightEditorPropertiesElements();
    void ReconfigureMapHeightEditorImGuiElements();

    void InitMapHeightEditor() {
        InitMapHeightEditorPropertiesElements();
    }

    void InitMapHeightEditorFileMenu() {

    }

    void InitMapHeightEditorPropertiesElements() {
        g_imguiElements.rendererSettingsHeader.SetTitle("Renderer Settings");
        g_imguiElements.drawGrass.SetText("Draw Grass");
        g_imguiElements.drawWater.SetText("Draw Water");
        g_imguiElements.mapHeightPropertiesHeader.SetTitle("Height Map Properties");
        g_imguiElements.mapHeightNameInput.SetLabel("Name");

        g_imguiElements.chunkWidth.SetText("Chunk Width");
        g_imguiElements.chunkDepth.SetText("Chunk Depth");

        g_imguiElements.minPaintHeight.SetText("Min Height");
        g_imguiElements.minPaintHeight.SetRange(0.0f, HEIGHTMAP_SCALE_Y);
        g_imguiElements.minPaintHeight.SetValue(0.0f);

        g_imguiElements.maxPaintHeight.SetText("Max Height");
        g_imguiElements.maxPaintHeight.SetRange(0.0f, HEIGHTMAP_SCALE_Y);
        g_imguiElements.maxPaintHeight.SetValue(HEIGHTMAP_SCALE_Y);

        g_imguiElements.brushStrength.SetText("Brush Strength");
        g_imguiElements.brushStrength.SetRange(0.0f, 1.0f);
        g_imguiElements.brushStrength.SetValue(g_brushStrength);

        g_imguiElements.noiseStrength.SetText("Noise Strength");
        g_imguiElements.noiseStrength.SetRange(0.0f, 1.0f);
        g_imguiElements.noiseStrength.SetValue(g_noiseStrength);

        g_imguiElements.noiseScale.SetText("Noise Scale");
        g_imguiElements.noiseScale.SetRange(0.0f, 1.0f);
        g_imguiElements.noiseScale.SetValue(g_noiseScale);

        g_imguiElements.brushSize.SetText("Brush Size");
        g_imguiElements.brushSize.SetRange(0.0f, 100.0f);
        g_imguiElements.brushSize.SetValue(g_brushSize);

        g_imguiElements.newFileWindow.SetTitle("New Height Map");
        g_imguiElements.newFileWindow.SetCallback(nullptr);
        g_imguiElements.openFileWindow.SetTitle("Open Height Map");
        g_imguiElements.openFileWindow.SetPath("res/height_maps/");
        g_imguiElements.openFileWindow.SetCallback(nullptr);
    }

    void ReconfigureMapHeightEditorImGuiElements() {
        //MapHeightEditorEditorImguiElements& elements = g_MapHeightEditorImguiElements;
        //
        //// Update name input with height map name
        //elements.MapHeightNameInput.SetText(SectorManager::GetSectorMapHeightName("MapHeightEditor_Center"));
        //
        //// Height map neighbor drop downs
        //std::vector<std::string> MapHeights = { "None" };
        //MapHeights.insert(MapHeights.end(), MapHeightManager::GetHeigthMapNames().begin(), MapHeightManager::GetHeigthMapNames().end());
        //
        //RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
        //elements.drawGrass.SetState(renderSettings.drawGrass);
    }

    void CreateMapHeightEditorImGuiElements() {
        BeginLeftPanel();

        // Renderer settings
        if (g_imguiElements.rendererSettingsHeader.CreateImGuiElement()) {
            if (g_imguiElements.drawGrass.CreateImGuiElements()) {
                RendererSettings& renderSettings = Renderer::GetCurrentRendererSettings();
                renderSettings.drawGrass = g_imguiElements.drawGrass.GetState();
            }
            if (g_imguiElements.drawWater.CreateImGuiElements()) {
                std::cout << g_imguiElements.drawWater.GetState();
            }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
        }

        // Height map properties
        if (g_imguiElements.mapHeightPropertiesHeader.CreateImGuiElement()) {
            g_imguiElements.mapHeightNameInput.CreateImGuiElement();

            if (g_imguiElements.chunkWidth.CreateImGuiElements()) {

            }

            if (g_imguiElements.chunkDepth.CreateImGuiElements()) {

            }

            if (g_imguiElements.brushSize.CreateImGuiElements()) {
                g_brushSize = g_imguiElements.brushSize.GetValue();
            }

            if (g_imguiElements.minPaintHeight.CreateImGuiElements()) {
                g_minPaintHeight = g_imguiElements.minPaintHeight.GetValue();
            }

            if (g_imguiElements.maxPaintHeight.CreateImGuiElements()) {
                g_maxPaintHeight = g_imguiElements.maxPaintHeight.GetValue();
            }

            if (g_imguiElements.brushStrength.CreateImGuiElements()) {
                g_brushStrength = g_imguiElements.brushStrength.GetValue();
            }

            if (g_imguiElements.noiseStrength.CreateImGuiElements()) {
                g_noiseStrength = g_imguiElements.noiseStrength.GetValue();
            }

            if (g_imguiElements.noiseScale.CreateImGuiElements()) {
                g_noiseScale = g_imguiElements.noiseScale.GetValue();
            }

            bool reloadRequired = false;

            ///////if (reloadRequired) {
            ///////    MapCreateInfo* mapCreateInfo = MapManager::GetMapCreateInfoByName("MapHeightEditorMap");
            ///////    World::LoadMap(mapCreateInfo);
            ///////    Renderer::RecalculateAllMapHeightData();
            ///////}
        }

        //g_imguiElements.leftPanel.EndImGuiElement();
        
        EndLeftPanel();

        // Windows
        if (g_imguiElements.newFileWindow.IsVisible()) {
            g_imguiElements.newFileWindow.CreateImGuiElements();
        }
        if (g_imguiElements.openFileWindow.IsVisible()) {
            g_imguiElements.openFileWindow.CreateImGuiElements();
        }
    }

    void OpenMapHeightEditor() {
        if (IsClosed()) {
            OpenEditor();
        }

        if (GetEditorMode() != EditorMode::MAP_HEIGHT_EDITOR) {
            SetEditorMode(EditorMode::MAP_HEIGHT_EDITOR);

            if (GetEditorMapName() != "Shit") {
                SetEditorMapName("Shit");
                World::LoadMapInstance("Shit");
            }

            InitFileMenuImGuiElements();
            InitLeftPanel();
            ReconfigureMapHeightEditorImGuiElements();

            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
        }
    }

    void UpdateMapHeightEditor() {
        int mapWidth = World::GetWorldSpaceWidth();
        int mapDepth = World::GetWorldSpaceDepth();

        int viewportIndex = Editor::GetHoveredViewportIndex();
        const Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        const glm::vec3 rayOrigin = Editor::GetMouseRayOriginByViewportIndex(viewportIndex);
        const glm::vec3 rayDir = Editor::GetMouseRayDirectionByViewportIndex(viewportIndex);

        ivecXZ hovered = ivecXZ(-1, -1);

        // Draw grid and check for mouse hover
        for (int x = 0; x < mapWidth; x++) {
            for (int z = 0; z < mapDepth; z++) {
                glm::vec3 p0 = glm::vec3((x + 0) * 64.0f, 0.0f, (z + 0) * 64.0f);
                glm::vec3 p1 = glm::vec3((x + 0) * 64.0f, 0.0f, (z + 1) * 64.0f);
                glm::vec3 p2 = glm::vec3((x + 1) * 64.0f, 0.0f, (z + 0) * 64.0f);
                glm::vec3 p3 = glm::vec3((x + 1) * 64.0f, 0.0f, (z + 1) * 64.0f);
                float t = 0;
                if (Util::RayIntersectsTriangle(rayOrigin, rayDir, p0, p1, p2, t) ||
                    Util::RayIntersectsTriangle(rayOrigin, rayDir, p1, p3, p2, t)) {
                    hovered = ivecXZ(x, z);
                }
                Renderer::DrawLine(p0, p1, GRID_COLOR, true);
                Renderer::DrawLine(p0, p2, GRID_COLOR, true);
                Renderer::DrawLine(p2, p3, GRID_COLOR, true);
                Renderer::DrawLine(p1, p3, GRID_COLOR, true);
            }
        }

        // Draw hovered
        if (hovered != ivecXZ(-1, -1)) {
            glm::vec3 p0 = glm::vec3((hovered.x + 0) * 64.0f, 0.0f, (hovered.z + 0) * 64.0f);
            glm::vec3 p1 = glm::vec3((hovered.x + 0) * 64.0f, 0.0f, (hovered.z + 1) * 64.0f);
            glm::vec3 p2 = glm::vec3((hovered.x + 1) * 64.0f, 0.0f, (hovered.z + 0) * 64.0f);
            glm::vec3 p3 = glm::vec3((hovered.x + 1) * 64.0f, 0.0f, (hovered.z + 1) * 64.0f);
            Renderer::DrawLine(p0, p1, WHITE, true);
            Renderer::DrawLine(p0, p2, WHITE, true);
            Renderer::DrawLine(p2, p3, WHITE, true);
            Renderer::DrawLine(p1, p3, WHITE, true);
        }
    }

    void ShowNewMapWindow() {
        CloseAllEditorWindows();
        g_imguiElements.newFileWindow.Show();
    }

    void ShowOpenMapWindow() {
        CloseAllEditorWindows();
        g_imguiElements.openFileWindow.Show();
    }

    void CloseAllMapHeightEditorWindows() {
        g_imguiElements.newFileWindow.Close();
        g_imguiElements.openFileWindow.Close();
    }

    float GetMapHeightNoiseScale() {
        return g_noiseScale;
    }

    float GetMapHeightBrushSize() {
        return g_brushSize;
    }

    float GetMapHeightBrushStrength() {
        return g_brushStrength;
    }

    float GetMapHeightNoiseStrength() {
        return g_noiseStrength;
    }

    float GetMapHeightMinPaintHeight() {
        return g_minPaintHeight;
    }

    float GetMapHeightMaxPaintHeight() {
        return g_maxPaintHeight;
    }
}