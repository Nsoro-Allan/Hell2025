#pragma once
#include "HellTypes.h"
#include "Callbacks/Callbacks.h"
#include "CreateInfo.h"
#include "Camera/Camera.h"
#include "Viewport/Viewport.h"
#include <string>

namespace Editor {

    struct PlacementObjectSubtype {
        GenericObjectType genericObject = GenericObjectType::UNDEFINED;
        FireplaceType fireplace = FireplaceType::UNDEFINED;
        HousePlaneType housePlane = HousePlaneType::UNDEFINED;

        void Reset() {
            genericObject = GenericObjectType::UNDEFINED;
            fireplace = FireplaceType::UNDEFINED;
            housePlane = HousePlaneType::UNDEFINED;
        }
    };

    void Init();
    void ResetCameras();
    void ResetViewports();
    void Update(float deltaTime);
    void UpdateCursor();
    void UpdateDividers();
    void UpdateInput();
    void UpdateUI();
    void UpdateCamera();
    void UpdateMouseRays();
    void UpdateCameraInterpolation(float deltaTime);
    void UpdateDebug();
    void OpenEditor();
    void CloseEditor();
    void ToggleEditorOpenState();
    void SetEditorMode(EditorMode editorMode);
    void SetActiveViewportIndex(int index);
    void SetSelectedObjectType(ObjectType editorObjectType);
    void SetHoveredObjectType(ObjectType editorObjectType);
    void SetSplitX(float value);
    void SetSplitY(float value);
    void SetViewportView(uint32_t viewportIndex, glm::vec3 viewportOrigin, CameraView targetView);
    void SetEditorSelectionMode(EditorSelectionMode editorSelectionMode);
    void SetEditorState(EditorState editorState);
    void SetViewportOrthographicState(uint32_t viewportIndex, bool state);
    void SetViewportOrthoSize(uint32_t viewportIndex, float size);
    void SetEditorViewportSplitMode(EditorViewportSplitMode mode);

    void SetPlantType(TreeType treeType);

    void UpdateGizmoInteract();

    float GetScalingFactor(int targetSizeInPixels);

    // Settings
    void SetBackfaceCulling(bool value);
    bool BackfaceCullingEnabled();
    bool BackfaceCullingDisabled();

    // Object hover
    void UpdateObjectHover();
    void SetHoveredObjectType(ObjectType objectType);
    void SetHoveredObjectId(uint64_t objectId);
    ObjectType GetHoveredObjectType();
    uint64_t GetHoveredObjectId();

    // Object selection
    void UnselectAnyObject();
    void UpdateObjectSelection();
    void SetSelectedObjectType(ObjectType objectType);
    void SetSelectedObjectId(uint64_t objectId);
    ObjectType GetSelectedObjectType();
    uint64_t GetSelectedObjectId();

    // Gizmo shit
    void UpdateObjectGizmoInteraction();

    // Axis constraint
    void ResetAxisConstraint();
    void SetAxisConstraint(Axis axis);
    Axis GetAxisConstraint();

    // File Menu
    void InitFileMenuImGuiElements();
    void CreateFileMenuImGuiElements();

    // Left panel
    void InitLeftPanel();
    void BeginLeftPanel();
    void EndLeftPanel();
    void UpdateOutliner();

    // New/Open
    void ShowNewMapWindow();
    void ShowOpenMapWindow();

    // House Editor
    void InitHouseEditor();
    void OpenHouseEditor();
    void UpdateHouseEditor();
    void ShowNewHouseWindow();
    void ShowOpenHouseWindow();
    void CloseAllHouseEditorWindows();
    void CreateHouseEditorImGuiElements();

    // Map Height Editor
    void InitMapHeightEditor();
    void OpenMapHeightEditor();
    void UpdateMapHeightEditor();
    void CloseAllMapHeightEditorWindows();
    void CreateMapHeightEditorImGuiElements();

    // Map Object Editor
    void OpenMapObjectEditor();
    void UpdateMapObjectEditor();
    void CreateMapObjectEditorImGuiElements();
    void ShowNewSectorWindow();
    void ShowOpenSectorWindow();
    void CloseAllMapObjectEditorWindows();

    float GetMapHeightNoiseScale();
    float GetMapHeightBrushSize();
    float GetMapHeightBrushStrength();
    float GetMapHeightNoiseStrength();
    float GetMapHeightMinPaintHeight();
    float GetMapHeightMaxPaintHeight();

    std::string GetNextEditorName(const std::string& desiredName, ObjectType objectType);

    std::string GetNextAvailableGenericObjectName(GenericObjectType type);
    std::string GetNextAvailableHousePlaneName(HousePlaneType type);
    std::string GetNextAvailableTreeName(TreeType type);
    std::string GetNextAvailableWallName();

    const std::vector<std::string>& GetCeilingNames();
    const std::vector<std::string>& GetGenericObjectNames();
    const std::vector<std::string>& GetFloorNames();
    const std::vector<std::string>& GetTreeNames();
    const std::vector<std::string>& GetUndefinedHousePlaneNames();
    const std::vector<std::string>& GetWallNames();

    void CloseAllEditorWindows();

    void Save();

    // Object placement
    //void PlaceObject(ObjectType objectType); // suss!!!!!!!!!!!
    void PlaceHousePlane(HousePlaneType housePlaneType);
    void PlaceFireplace(FireplaceType fireplaceType);
    void PlaceGenericObject(GenericObjectType objectType);
    ObjectType GetPlacementObjectType();
    //GenericObjectType GetGenericPlacementObjectType();
    PlacementObjectSubtype GetPlacementObjectSubtype();
    void ResetPlacementObjectSubtype();

    void UpdateDoorPlacement();
    void UpdatePictureFramePlacement();
    void UpdatePlayerCampaignSpawnPlacement();
    void UpdatePlayerDeathmatchSpawnPlacement();
    void UpdateTreePlacement();
    void UpdateWallPlacement();
    void UpdateWindowPlacement();
    void UpdateObjectPlacement();
    void ExitObjectPlacement();
    void SetPlacementObjectId(uint64_t objectId);

    // Util
    PhysXRayResult GetMouseRayPhsyXHitPosition();

    uint64_t GetPlacementObjectId();

    // Ray intersections
    glm::vec3 GetMouseRayPlaneIntersectionPoint(glm::vec3 planeOrigin, glm::vec3 planeNormal);

    int GetActiveViewportIndex();
    int GetHoveredViewportIndex();
    bool IsOpen();
    bool IsClosed();
    bool IsViewportOrthographic(uint32_t viewportIndex);
    bool EditorIsIdle();
    bool EditorWasIdleLastFrame();
    float GetVerticalDividerXPos();
    float GetHorizontalDividerYPos();
    glm::vec3 GetMouseRayOriginByViewportIndex(int32_t viewportIndex);
    glm::vec3 GetMouseRayDirectionByViewportIndex(int32_t viewportIndex);
    glm::mat4 GetViewportViewMatrix(int32_t viewportIndex);
    float GetEditorOrthoSize(int32_t viewportIndex);
    Viewport* GetActiveViewport();
    ShadingMode GetViewportModeByIndex(uint32_t index);
    CameraView GetCameraViewByIndex(uint32_t index);
    EditorState GetEditorState();
    EditorSelectionMode GetEditorSelectionMode();
    EditorViewportSplitMode GetEditorViewportSplitMode();
    SelectionRectangleState& GetSelectionRectangleState();
    EditorMode& GetEditorMode();
    Axis GetAxisConstraint();

    void SetEditorHouseName(const std::string& houseName);
    void SetEditorMapName(const std::string& mapName);
    const std::string& GetEditorHouseName();
    const std::string& GetEditorMapName();

    // Dividers
    bool IsVerticalDividerHovered();
    bool IsHorizontalDividerHovered();
}