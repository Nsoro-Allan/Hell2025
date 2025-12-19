#include "MagicEnum.hpp"
#include "Util.h"

namespace Util {
    BlendingMode StringToBlendingMode(const std::string& str)                   { return magic_enum::enum_cast<BlendingMode>(str).value_or(BlendingMode::DEFAULT); }
    CameraView StringToCameraView(const std::string& str)                       { return magic_enum::enum_cast<CameraView>(str).value_or(CameraView::UNDEFINED); }
    DebugRenderMode StringToDebugRenderMode(const std::string& str)             { return magic_enum::enum_cast<DebugRenderMode>(str).value_or(DebugRenderMode::NONE); }
    DoorType StringToDoorType(const std::string& str)                           { return magic_enum::enum_cast<DoorType>(str).value_or(DoorType::UNDEFINED); }
    DoorMaterialType StringToDoorMaterialType(const std::string& str)           { return magic_enum::enum_cast<DoorMaterialType>(str).value_or(DoorMaterialType::UNDEFINED); }
    EditorMode StringToEditorMode(const std::string& str)                       { return magic_enum::enum_cast<EditorMode>(str).value_or(EditorMode::UNDEFINED); }
    EditorSelectionMode StringToEditorSelectionMode(const std::string& str)     { return magic_enum::enum_cast<EditorSelectionMode>(str).value_or(EditorSelectionMode::OBJECT); }
    EditorState StringToEditorState(const std::string& str)                     { return magic_enum::enum_cast<EditorState>(str).value_or(EditorState::IDLE); }
    FireplaceType StringToFireplaceType(const std::string& str)                 { return magic_enum::enum_cast<FireplaceType>(str).value_or(FireplaceType::UNDEFINED); }
    GenericObjectType StringToGenericObjectType(const std::string& str)         { return magic_enum::enum_cast<GenericObjectType>(str).value_or(GenericObjectType::UNDEFINED); }
    HousePlaneType StringToHousePlaneType(const std::string& str)               { return magic_enum::enum_cast<HousePlaneType>(str).value_or(HousePlaneType::UNDEFINED); }
    HouseType StringToHouseType(const std::string& str)                         { return magic_enum::enum_cast<HouseType>(str).value_or(HouseType::UNDEFINED); }
    ImageDataType StringToImageDataType(const std::string& str)                 { return magic_enum::enum_cast<ImageDataType>(str).value_or(ImageDataType::UNDEFINED); }
    InventoryState StringToInventoryState(const std::string& str)               { return magic_enum::enum_cast<InventoryState>(str).value_or(InventoryState::UNDEFINED); }
    LightType StringToLightType(const std::string& str)                         { return magic_enum::enum_cast<LightType>(str).value_or(LightType::UNDEFINED); }
    ObjectType StringToObjectType(const std::string& str)                       { return magic_enum::enum_cast<ObjectType>(str).value_or(ObjectType::UNDEFINED); }
    OpenState StringToOpenState(const std::string& str)                         { return magic_enum::enum_cast<OpenState>(str).value_or(OpenState::UNDEFINED); }
    PickUpTypeOld StringToPickUpType(const std::string& str)                       { return magic_enum::enum_cast<PickUpTypeOld>(str).value_or(PickUpTypeOld::UNDEFINED); }
    PhysicsType StringToPhysicsType(const std::string& str)                     { return magic_enum::enum_cast<PhysicsType>(str).value_or(PhysicsType::UNDEFINED); }
    PictureFrameType StringToPictureFrameType(const std::string& str)           { return magic_enum::enum_cast<PictureFrameType>(str).value_or(PictureFrameType::UNDEFINED); }
    SharkHuntingState StringToSharkHuntingState(const std::string& str)         { return magic_enum::enum_cast<SharkHuntingState>(str).value_or(SharkHuntingState::UNDEFINED); }
    SharkMovementState StringToSharkMovementState(const std::string& str)       { return magic_enum::enum_cast<SharkMovementState>(str).value_or(SharkMovementState::UNDEFINED); }
    TreeType StringToTreeType(const std::string& str)                           { return magic_enum::enum_cast<TreeType>(str).value_or(TreeType::UNDEFINED); }
    TrimType StringToTrimType(const std::string& str)                           { return magic_enum::enum_cast<TrimType>(str).value_or(TrimType::UNDEFINED); }
    ShadingMode StringToViewportMode(const std::string& str)                    { return magic_enum::enum_cast<ShadingMode>(str).value_or(ShadingMode::SHADED); }
    WallType StringToWallType(const std::string& str)                           { return magic_enum::enum_cast<WallType>(str).value_or(WallType::UNDEFINED); }
    WeaponAction StringToWeaponAction(const std::string& str)                   { return magic_enum::enum_cast<WeaponAction>(str).value_or(WeaponAction::UNDEFINED); }

    std::string BlendingModeToString(BlendingMode mode)                         { return std::string(magic_enum::enum_name(mode)); }
    std::string CameraViewToString(CameraView cameraView)                       { return std::string(magic_enum::enum_name(cameraView)); }
    std::string DebugRenderModeToString(DebugRenderMode mode)                   { return std::string(magic_enum::enum_name(mode)); }
    std::string DoorTypeToString(DoorType type)                                 { return std::string(magic_enum::enum_name(type)); }
    std::string DoorMaterialTypeToString(DoorMaterialType type)                 { return std::string(magic_enum::enum_name(type)); }
    std::string EditorModeToString(EditorMode editorMode)                       { return std::string(magic_enum::enum_name(editorMode)); }
    std::string EditorSelectionModeToString(EditorSelectionMode mode)           { return std::string(magic_enum::enum_name(mode)); }
    std::string EditorStateToString(EditorState state)                          { return std::string(magic_enum::enum_name(state)); }
    std::string GenericObjectTypeToString(GenericObjectType houseType)          { return std::string(magic_enum::enum_name(houseType)); }
    std::string FireplaceTypeToString(FireplaceType type)                       { return std::string(magic_enum::enum_name(type)); }
    std::string HousePlaneTypeToString(HousePlaneType type)                     { return std::string(magic_enum::enum_name(type)); }
    std::string HouseTypeToString(HouseType houseType)                          { return std::string(magic_enum::enum_name(houseType)); }
    std::string ImageDataTypeToString(ImageDataType imageDataType)              { return std::string(magic_enum::enum_name(imageDataType)); }
    std::string InventoryStateToString(InventoryState state)                    { return std::string(magic_enum::enum_name(state)); }
    std::string LightTypeToString(LightType type)                               { return std::string(magic_enum::enum_name(type)); }
    std::string ObjectTypeToString(ObjectType type)                             { return std::string(magic_enum::enum_name(type)); }
    std::string OpenStateToString(OpenState mode)                               { return std::string(magic_enum::enum_name(mode)); }
    std::string PickUpTypeToString(PickUpType type)                             { return std::string(magic_enum::enum_name(type)); }
    std::string PhysicsTypeToString(PhysicsType type)                           { return std::string(magic_enum::enum_name(type)); }
    std::string PictureFrameTypeToString(PictureFrameType type)                 { return std::string(magic_enum::enum_name(type)); }
    std::string SharkHuntingStateToString(SharkHuntingState state)              { return std::string(magic_enum::enum_name(state)); }
    std::string SharkMovementStateToString(SharkMovementState state)            { return std::string(magic_enum::enum_name(state)); }
    std::string TreeTypeToString(TreeType type)                                 { return std::string(magic_enum::enum_name(type)); }
    std::string TrimTypeToString(TrimType type)                                 { return std::string(magic_enum::enum_name(type)); }
    std::string ViewportModeToString(ShadingMode viewportMode)                  { return std::string(magic_enum::enum_name(viewportMode)); }
    std::string WallTypeToString(WallType type)                                 { return std::string(magic_enum::enum_name(type)); }
    std::string WeaponActionToString(WeaponAction weaponAction)                 { return std::string(magic_enum::enum_name(weaponAction)); }

    ObjectType IntToEnum(int value) {
        return static_cast<ObjectType>(value);
    }

    int32_t EnumToInt(ObjectType type) {
        return static_cast<int32_t>(type);
    }

    std::string FloatToString(float value, int precision) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), ("%." + std::to_string(precision) + "f").c_str(), value);
        return std::string(buffer);
    }

    std::string DoubleToString(double value, int precision) {
        char buffer[64];
        std::string fmt = "%." + std::to_string(precision) + "f";
        std::snprintf(buffer, sizeof(buffer), fmt.c_str(), value);
        return std::string(buffer);
    }
}
