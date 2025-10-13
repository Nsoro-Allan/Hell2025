#include "Editor.h"
#include "HellDefines.h"
#include "Callbacks/Callbacks.h"
#include "Gizmo.h"
#include "BackEnd/BackEnd.h"
#include "Camera/Camera.h"
#include "Config/Config.h"
#include "Audio/Audio.h"
#include "Core/Debug.h"
#include "Core/Game.h"
#include "Imgui/ImguiBackEnd.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Managers/HouseManager.h"
#include "Managers/MapManager.h"
#include "Renderer/Renderer.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"
#include "World/SectorManager.h"

namespace Editor {
    EditorMode g_editorMode = EditorMode::UNDEFINED;
    int g_activeViewportIndex = 3;
    bool g_isOpen = false;
    bool g_isOrthographic[4];
    bool g_editorStateWasIdleLastFrame = true;
    float g_OrthographicSizes[4];
    float g_verticalDividerXPos = 0.2f;
    float g_horizontalDividerYPos = 0.2f;

    ObjectType g_hoveredObjectType = ObjectType::NONE;
    ObjectType g_selectedObjectType = ObjectType::NONE;
    ObjectType g_placementObjectType = ObjectType::NONE;
    uint64_t g_hoveredObjectId = 0;
    uint64_t g_selectedObjectId = 0;

    ShadingMode g_shadingModes[4];
    EditorViewportSplitMode g_editorViewportSplitMode = EditorViewportSplitMode::SINGLE;
    Axis g_axisConstraint = Axis::NONE;

    std::string g_editorHouseName = UNDEFINED_STRING;
    std::string g_editorMapName = UNDEFINED_STRING;

    float g_orthoCameraDistances[4];
    EditorState g_editorState;
    SelectionRectangleState g_viewportSelectionRectangleState;

    void Init() {
        ResetViewports();
        ResetCameras();

        //InitMapObjectEditor();
        InitMapHeightEditor();
        InitHouseEditor();
    }

    void OpenEditor() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Editor::SetEditorState(EditorState::IDLE);
        Editor::ResetAxisConstraint();
        Input::ShowCursor();
        Input::CenterMouseCursor();

        // Load default map
        if (GetEditorMapName() != "Shit") {
            SetEditorMapName("Shit");
        }

        // Load default house
        if (GetEditorHouseName() != "TestHouse") {
            SetEditorHouseName("TestHouse");
        }

        g_isOpen = true;
    }

    void CloseEditor() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::DisableCursor();

        UnselectAnyObject();
        SetHoveredObjectType(ObjectType::NONE);
        SetHoveredObjectId(0);

        if (GetEditorMode() == EditorMode::HOUSE_EDITOR) {
            // Update the house file with everything in the world
            HouseManager::UpdateCreateInfoCollectionFromWorld(Editor::GetEditorHouseName());

            // Reload the single editor house
            World::ClearAllObjects();
            World::LoadSingleHouse(Editor::GetEditorHouseName());
        }
        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) {
            // Reload everything from the editor map file
            Renderer::RecalculateAllHeightMapData(false);
            World::ClearAllObjects();
            World::LoadMapInstanceObjects(Editor::GetEditorMapName(), SpawnOffset());
            World::LoadMapInstanceHouses(Editor::GetEditorMapName(), SpawnOffset());
            World::RecreateHouseMesh();
        }
        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            // Update the map file with everything in the world
            MapManager::UpdateCreateInfoCollectionFromWorld(Editor::GetEditorMapName());

            // Reload everything from the editor map file, except the heightmap
            World::ClearAllObjects();
            World::LoadMapInstanceObjects(Editor::GetEditorMapName(), SpawnOffset());
            World::LoadMapInstanceHouses(Editor::GetEditorMapName(), SpawnOffset());
            World::RecreateHouseMesh();
        }

        g_isOpen = false;
    }

    void Save() {
        if (Editor::GetEditorMode() == EditorMode::HOUSE_EDITOR) {
            HouseManager::SaveHouse(Editor::GetEditorHouseName());
        }
        else if (Editor::GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR || Editor::GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            MapManager::SaveMap(Editor::GetEditorMapName());
        }
    }

    void ResetViewports() {

        const Resolutions& resolutions = Config::GetResolutions();

        // Center the viewport splits
        float editorWidth = resolutions.ui.x - EDITOR_LEFT_PANEL_WIDTH;
        float editorHeight = resolutions.ui.y - ImGuiBackEnd::GetFileMenuHeight();
        float editorWidthNormalized = editorWidth / resolutions.ui.x;
        float editorHeightNormalized = editorHeight / resolutions.ui.y;
        float panelRightEdgeNormalized = EDITOR_LEFT_PANEL_WIDTH / resolutions.ui.x;
        float fileMenuHeightNormalized = ImGuiBackEnd::GetFileMenuHeight() / resolutions.ui.y;
        g_verticalDividerXPos = panelRightEdgeNormalized + (editorWidthNormalized * 0.5f);
        g_horizontalDividerYPos = fileMenuHeightNormalized + (editorHeightNormalized * 0.5f);

        float ORTHO_CAMERA_DISTANCE_FROM_ORIGIN = 250.0f;
        g_orthoCameraDistances[0] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[1] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[2] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
        g_orthoCameraDistances[3] = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;

        // Top left
        SetViewportView(0, Gizmo::GetPosition(), CameraView::RIGHT);
        //ViewportManager::GetViewportByIndex(0)->SetOrthoSize(1.14594f);
        ViewportManager::GetViewportByIndex(0)->SetOrthoSize(76.1911f);

        // Top right
        SetViewportView(1, Gizmo::GetPosition(), CameraView::LEFT);
        ViewportManager::GetViewportByIndex(1)->SetOrthoSize(1.1958f);

        // Bottom left
        SetViewportView(2, Gizmo::GetPosition(), CameraView::TOP);
        ViewportManager::GetViewportByIndex(2)->SetOrthoSize(1.19627f);

        // Bottom right
        SetViewportView(3, Gizmo::GetPosition(), CameraView::FRONT);
        ViewportManager::GetViewportByIndex(3)->SetOrthoSize(1.1958f);
    }

    void Update(float deltaTime) {
        // Close editor
        if (Input::KeyPressed(HELL_KEY_TAB)) {
            InputMulti::ClearKeyStates();
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            CloseEditor();
        }

        if (!IsOpen()) {
            return;
        }

        g_editorStateWasIdleLastFrame = (g_editorState == EditorState::IDLE);

        UpdateCamera();    // you swapped these two, maybe it was better before?
        UpdateMouseRays(); // you swapped these two, maybe it was better before?
        UpdateObjectHover();
        UpdateObjectSelection();
        UpdateObjectGizmoInteraction();
        UpdateDividers();
        UpdateInput();
        UpdateUI();
        UpdateCursor();
        UpdateDebug();
        UpdateCameraInterpolation(deltaTime);
        Gizmo::Update();

        if (GetEditorMode() == EditorMode::HOUSE_EDITOR || GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            UpdateObjectPlacement();
        }
        if (GetEditorState() == EditorState::IDLE) {
            ExitObjectPlacement();
        }

        UpdateOutliner(); // make this nicer

        switch (GetEditorMode()) {
            case EditorMode::HOUSE_EDITOR:      UpdateHouseEditor();       break;
            case EditorMode::MAP_HEIGHT_EDITOR: UpdateMapHeightEditor();   break;
            case EditorMode::MAP_OBJECT_EDITOR: UpdateMapObjectEditor();   break;
            default: break;
        }

        if (Input::KeyPressed(HELL_KEY_Q)) {
            if (GetEditorViewportSplitMode() == EditorViewportSplitMode::SINGLE) {
                SetEditorViewportSplitMode(EditorViewportSplitMode::FOUR_WAY_SPLIT);
                std::cout << "four way\n";
            }
            else {
                SetEditorViewportSplitMode(EditorViewportSplitMode::SINGLE);
                std::cout << "single\n";
            }
        }

        // Draw grid and spawn points
        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR || GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            float h = 30.0f;
            float w = World::GetWorldSpaceWidth();
            float d = World::GetWorldSpaceDepth();

            // Draw perimeter
            glm::vec3 p0 = glm::vec3(0.0f, h, 0.0f);
            glm::vec3 p1 = glm::vec3(w, h, 0.0f);
            glm::vec3 p2 = glm::vec3(0.0f, h, d);
            glm::vec3 p3 = glm::vec3(w, h, d);
            Renderer::DrawLine(p0, p1, GRID_COLOR, true);
            Renderer::DrawLine(p0, p2, GRID_COLOR, true);
            Renderer::DrawLine(p2, p3, GRID_COLOR, true);
            Renderer::DrawLine(p1, p3, GRID_COLOR, true);

            // Draw spawn points as little dots
            Map* map = MapManager::GetMapByName(GetEditorMapName());
            if (map) {
                for (SpawnPoint& spawnPoints : map->GetAdditionalMapData().playerCampaignSpawns) {
                    Renderer::DrawPoint(spawnPoints.GetPosition(), GREEN);
                }
                for (SpawnPoint& spawnPoints : map->GetAdditionalMapData().playerDeathmatchSpawns) {
                    Renderer::DrawPoint(spawnPoints.GetPosition(), YELLOW);
                }
            }

            // Draw cubes around spawn points
            for (SpawnPoint& spawnPoint : World::GetCampaignSpawnPoints()) {
                spawnPoint.DrawDebugCube();
            }
        }
    }

    void ToggleEditorOpenState() {
        g_isOpen = !g_isOpen;
        if (g_isOpen) {
            OpenEditor();
        }
        else {
            CloseEditor();
        }
    }

    void PlaceObject(ObjectType objectType) {
        SetEditorState(EditorState::PLACE_OBJECT);
        g_placementObjectType = objectType;
    }

    ObjectType GetPlacementObjectType() {
        return g_placementObjectType;
    }

    void SetEditorMode(EditorMode editorMode) {
        g_editorMode = editorMode;
    }

    void SetActiveViewportIndex(int index) {
        g_activeViewportIndex = index;
    }

    int GetActiveViewportIndex() {
        return g_activeViewportIndex;
    }

    int GetHoveredViewportIndex() {
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible() && viewport->IsHovered()) {
                return i;
            }
        }
        return 0;
    }

    bool IsOpen() {
        return g_isOpen;
    }

    bool IsClosed() {
        return !g_isOpen;
    }

    bool EditorIsIdle() {
        return g_editorState == EditorState::IDLE;
    }

    bool EditorWasIdleLastFrame() {
        return g_editorStateWasIdleLastFrame;
    }

    //void SetSelectedObjectIndex(int index) {
    //    g_selectedObjectIndex = index;
    //}
    //
    //void SetHoveredObjectIndex(int index) {
    //    g_hoveredObjectIndex = index;
    //}


    void SetSplitX(float value) {
        g_verticalDividerXPos = value;
        //ViewportManager::UpdateViewports();
    }

    void SetSplitY(float value) {
        g_horizontalDividerYPos = value;
        //ViewportManager::UpdateViewports();
    }

    //void SetCurrentMapName(const std::string& filename) {
    //    g_currentMapName = filename;
    //}

    void SetHoveredObjectType(ObjectType objectType) {
        g_hoveredObjectType = objectType;
    }

    void SetHoveredObjectId(uint64_t objectId) {
        g_hoveredObjectId = objectId;
    }

    void SetSelectedObjectType(ObjectType objectType) {
        g_selectedObjectType = objectType;
    }

    void SetSelectedObjectId(uint64_t objectId) {
        g_selectedObjectId = objectId;
    }

    ObjectType GetSelectedObjectType() {
        return g_selectedObjectType;
    }

    ObjectType GetHoveredObjectType() {
        return g_hoveredObjectType;
    }

    uint64_t GetSelectedObjectId() {
        return g_selectedObjectId;
    }

    uint64_t GetHoveredObjectId() {
        return g_hoveredObjectId;
    }

    bool IsViewportOrthographic(uint32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_isOrthographic[viewportIndex];
        }
        else {
            std::cout << "Game::GetCameraByIndex(int index) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
            return false;
        }
    }

    //Camera* GetCameraByIndex(uint32_t index) {
    //    if (index >= 0 && index < 4) {
    //        return &g_cameras[index];
    //    }
    //    else {
    //        std::cout << "Game::GetCameraByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
    //        return nullptr;
    //    }
    //}

    Viewport* GetActiveViewport() {
        if (g_activeViewportIndex >= 0 && g_activeViewportIndex < 4) {
            return ViewportManager::GetViewportByIndex(g_activeViewportIndex);
        }
        else {
            std::cout << "Editor::GetActiveViewport(int index) failed. " << g_activeViewportIndex << " out of range of editor viewport count 4\n";
            return nullptr;
        }
    }

    ShadingMode GetViewportModeByIndex(uint32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_shadingModes[viewportIndex];
        }
        else {
            std::cout << "Editor::GetViewportModeByIndex(uint32_t viewportIndex) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
            return ShadingMode::SHADED;
        }
    }

    //void SetCameraView(uint32_t cameraViewIndex, CameraView cameraView) {
    //    if (cameraViewIndex >= 0 && cameraViewIndex < 4) {
    //        g_cameraViews[cameraViewIndex] = cameraView;
    //    }
    //    else {
    //        std::cout << "Editor::SetCameraViewByIndex(uint32_t cameraViewIndex, CameraView cameraView) failed. " << cameraViewIndex << " out of range of editor viewport count 4\n";
    //    }
    //}

    void SetEditorViewportSplitMode(EditorViewportSplitMode mode) {
        g_editorViewportSplitMode = mode;
    }

    void SetViewportOrthoSize(uint32_t viewportIndex, float size) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            g_OrthographicSizes[viewportIndex] = size;
        }
        else {
            std::cout << "Editor::SetViewportOrthoSize(uint32_t viewportIndex, float size) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
        }
    }

    EditorState GetEditorState() {
        return g_editorState;
    }

    SelectionRectangleState& GetSelectionRectangleState() {
        return g_viewportSelectionRectangleState;
    }

    EditorMode& GetEditorMode() {
        return g_editorMode;
    }

    float GetVerticalDividerXPos() {
        return g_verticalDividerXPos;
    }

    float GetHorizontalDividerYPos() {
        return g_horizontalDividerYPos;
    }

    EditorViewportSplitMode GetEditorViewportSplitMode() {
        return g_editorViewportSplitMode;
    }

    void SetEditorState(EditorState editorState) {
        g_editorState = editorState;
    }

    void SetViewportOrthographicState(uint32_t index, bool state) {
        if (index >= 0 && index < 4) {
            g_isOrthographic[index] = state;
        }
        else {
            std::cout << "Editor::SetViewportOrthographicStateByIndex(uint32_t index, bool state) failed. " << index << " out of range of editor viewport count 4\n";
        }
    }

    void SetAxisConstraint(Axis axis) {
        g_axisConstraint = axis;
    }

    void ResetAxisConstraint() {
        g_axisConstraint = Axis::NONE;
    }

    Axis GetAxisConstraint() {
        return g_axisConstraint;
    }

    void CloseAllEditorWindows() {
        CloseAllHouseEditorWindows();
        CloseAllMapHeightEditorWindows();
        CloseAllMapObjectEditorWindows();
    }

    void SetEditorHouseName(const std::string& houseName) {
        g_editorHouseName = houseName;
    }

    void SetEditorMapName(const std::string& mapName) {
        g_editorMapName = mapName;
    }

    const std::string& GetEditorHouseName() {
        return g_editorHouseName;
    }

    const std::string& GetEditorMapName() {
        return g_editorMapName;
    }
}