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
    uint64_t g_hoveredObjectId = 0;
    uint64_t g_selectedObjectId = 0;

    ShadingMode g_shadingModes[4];
    EditorViewportSplitMode g_editorViewportSplitMode = EditorViewportSplitMode::SINGLE;
    Axis g_axisConstraint = Axis::NONE;
    
    std::string g_editorMapName = UNDEFINED_STRING;

    float g_orthoCameraDistances[4];
    EditorState g_editorState;
    SelectionRectangleState g_viewportSelectionRectangleState;

    void Init() {
        ResetViewports();
        ResetCameras();

        InitMapObjectEditor();
        InitMapHeightEditor();
        InitHouseEditor();
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

        // Toggle editor
        if (Input::KeyPressed(HELL_KEY_J)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            Editor::ToggleEditorOpenState();
        }

        if (Input::KeyPressed(HELL_KEY_F1)) {
            Callbacks::NewRun();
        }
        if (Input::KeyPressed(HELL_KEY_F4)) {
            Callbacks::OpenHouseEditor();
        }
        if (Input::KeyPressed(HELL_KEY_F6)) {
            Callbacks::OpenMapHeightEditor();
        }
        if (Input::KeyPressed(HELL_KEY_F5)) {
            Callbacks::OpenMapObjectEditor();
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
    }

    void OpenEditor() {
        std::cout << "Entered editor\n";
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::ShowCursor();
        Input::CenterMouseCursor();

        Editor::SetEditorState(EditorState::IDLE);
        Editor::ResetAxisConstraint();

        g_isOpen = true;
    }

    void CloseEditor() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::DisableCursor();

        UnselectAnyObject();
        SetHoveredObjectType(ObjectType::NONE);
        SetHoveredObjectId(0);

        if (GetEditorMode() == EditorMode::MAP_HEIGHT_EDITOR) {
            World::LoadMapInstanceObjects("Shit", SpawnOffset());
            Renderer::RecalculateAllHeightMapData(false);
        } 

        if (GetEditorMode() == EditorMode::MAP_OBJECT_EDITOR) {
            World::LoadMapInstanceObjects("Shit", SpawnOffset());
        }
        g_isOpen = false;
    }

    void ToggleEditorOpenState() {
        g_isOpen = !g_isOpen;
        if (g_isOpen) {
            OpenEditor();
            switch (GetEditorMode()) {
                case EditorMode::HOUSE_EDITOR:      OpenHouseEditor();      break;
                case EditorMode::MAP_HEIGHT_EDITOR: OpenMapHeightEditor();  break;
                case EditorMode::MAP_OBJECT_EDITOR: OpenMapObjectEditor();  break;
                default:                            OpenMapObjectEditor();  break;
            }
        }
        else {
            CloseEditor();
        }
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

    void SetEditorMapName(const std::string& mapName) {
        g_editorMapName = mapName;
    }

    const std::string& GetEditorMapName() {
        return g_editorMapName;
    }
}