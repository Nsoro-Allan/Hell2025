#include "Util.h"

namespace Util {
    std::string InventoryStateToString(InventoryState state) {
        switch (state) {
            case InventoryState::CLOSED:            return "CLOSED";
            case InventoryState::MAIN_SCREEN:       return "MAIN_SCREEN";
            case InventoryState::EXAMINE_ITEM:      return "EXAMINE_ITEM";
            case InventoryState::MOVING_ITEM:       return "MOVING_ITEM";
            case InventoryState::ROTATING_ITEM:     return "ROTATING_ITEM";
            default: return "UNKNOWN";
        }
    }

    std::string DrawersTypeToString(DrawersType houseType) {
        switch (houseType) {
            case DrawersType::SMALL:        return "SMALL";
            case DrawersType::LARGE:        return "LARGE";
            case DrawersType::UNDEFINED:    return "UNDEFINED";
            default: return "UNKNOWN";
        }
    }

    std::string LightTypeToString(LightType type) {
        switch (type) {
            case LightType::LAMP_POST:      return "LAMP_POST";
            case LightType::HANGING_LIGHT:  return "HANGING_LIGHT";
            default: return "UNKNOWN";
        }
    }

    LightType StringToLightType(const std::string& str) {
        if (str == "LAMP_POST")         return LightType::LAMP_POST;
        if (str == "HANGING_LIGHT")     return LightType::HANGING_LIGHT;
        return LightType::LAMP_POST;
    }

    std::string PickUpTypeToString(PickUpType type) {
        switch (type) {
            case PickUpType::UNDEFINED:             return "UNDEFINED";
            case PickUpType::SHOTGUN_AMMO_BUCKSHOT: return "SHOTGUN_AMMO_BUCKSHOT";
            case PickUpType::SHOTGUN_AMMO_SLUG:     return "SHOTGUN_AMMO_SLUG";
            case PickUpType::AKS74U:                return "AKS74U";
            case PickUpType::REMINGTON_870:         return "REMINGTON_870";
            case PickUpType::SPAS:                  return "SPAS";
            case PickUpType::GLOCK:                 return "GLOCK";
            case PickUpType::GOLDEN_GLOCK:          return "GOLDEN_GLOCK";
            case PickUpType::TOKAREV:               return "TOKAREV";
            default: return "UNKNOWN";
        }
    }

    PickUpType StringToPickUpType(const std::string& str) {
        if (str == "SHOTGUN_AMMO_BUCKSHOT") return PickUpType::SHOTGUN_AMMO_BUCKSHOT;
        if (str == "SHOTGUN_AMMO_SLUG")     return PickUpType::SHOTGUN_AMMO_SLUG;
        if (str == "AKS74U")                return PickUpType::AKS74U;
        if (str == "REMINGTON_870")         return PickUpType::REMINGTON_870;
        if (str == "GLOCK")                 return PickUpType::GLOCK;
        if (str == "GOLDEN_GLOCK")          return PickUpType::GOLDEN_GLOCK;
        if (str == "SPAS")                  return PickUpType::SPAS;
        if (str == "TOKAREV")               return PickUpType::TOKAREV;
        return PickUpType::UNDEFINED;
    }

    PictureFrameType StringToPictureFrameType(const std::string& str) {
        if (str == "BIG_LANDSCAPE")         return PictureFrameType::BIG_LANDSCAPE;
        if (str == "TALL_THIN")             return PictureFrameType::TALL_THIN;
        if (str == "REGULAR_PORTRAIT")      return PictureFrameType::REGULAR_PORTRAIT;
        if (str == "REGULAR_LANDSCAPE")     return PictureFrameType::REGULAR_LANDSCAPE;
        return PictureFrameType::UNDEFINED;
    }

    TrimType StringToTrimType(const std::string& str) {
        if (str == "PLASTER") return TrimType::PLASTER;
        if (str == "TIMBER") return TrimType::TIMBER;
        return TrimType::NONE;
    }

    WallType StringToWallType(const std::string& str) {
        if (str == "INTERIOR") return WallType::INTERIOR;
        if (str == "WEATHER_BOARDS") return WallType::WEATHER_BOARDS;
        return WallType::NONE;
    }

    std::string BlendingModeToString(BlendingMode mode) {
        switch (mode) {
            case BlendingMode::BLEND_DISABLED:      return "BLEND_DISABLED";
            case BlendingMode::BLENDED:             return "BLENDED";
            case BlendingMode::ALPHA_DISCARDED:     return "ALPHA_DISCARDED";
            case BlendingMode::HAIR_UNDER_LAYER:    return "HAIR_UNDER_LAYER";
            case BlendingMode::HAIR_TOP_LAYER:      return "HAIR_TOP_LAYER";
            case BlendingMode::DO_NOT_RENDER:       return "DO_NOT_RENDER";
            default: return "UNKNOWN";
        }
    }

    std::string HouseTypeToString(HouseType houseType) {
        switch (houseType) {
            case HouseType::SMALL_HOUSE:    return "SMALL_HOUSE";
            case HouseType::MEDIUM_HOUSE:   return "MEDIUM_HOUSE";
            case HouseType::LARGE_HOUSE:    return "LARGE_HOUSE";
            case HouseType::NAMED:          return "NAMED";
            case HouseType::UNDEFINED:      return "UNDEFINED";
            default: return "UNKNOWN";
        }
    }

    TreeType StringToTreeType(const std::string& str) {
        if (str == "TREE_LARGE_0")   return TreeType::TREE_LARGE_0;
        if (str == "TREE_LARGE_1")   return TreeType::TREE_LARGE_1;
        if (str == "TREE_LARGE_2")   return TreeType::TREE_LARGE_2;
        if (str == "BLACK_BERRIES")  return TreeType::BLACK_BERRIES;
        return TreeType::UNDEFINED;
    }

    std::string TreeTypeToString(TreeType type) {
        switch (type) {
            case TreeType::TREE_LARGE_0:  return "TREE_LARGE_0";
            case TreeType::TREE_LARGE_1:  return "TREE_LARGE_1";
            case TreeType::TREE_LARGE_2:  return "TREE_LARGE_2";
            case TreeType::BLACK_BERRIES: return "BLACK_BERRIES";
            case TreeType::UNDEFINED:     return "UNDEFINED";
            default:                      return "UNKNOWN";
        }
    }

    std::string OpenStateToString(OpenState mode) {
        switch (mode) {
            case OpenState::OPEN:               return "OPEN";
            case OpenState::OPENING:            return "OPENING";
            case OpenState::CLOSED:             return "CLOSED";
            case OpenState::CLOSING:            return "CLOSING";
            default: return "UNKNOWN";
        }
    }

    std::string ObjectTypeToString(ObjectType type) {
        switch (type) {
            case ObjectType::NONE:                          return "NONE";
            case ObjectType::DECAL:                         return "DECAL";
            case ObjectType::CHARACTER_CONTROLLER:          return "CHARACTER_CONTROLLER";
            case ObjectType::DOOR:                          return "DOOR";
            case ObjectType::DRAWERS:                       return "DRAWERS";
            case ObjectType::DOOR_FRAME:                    return "DOOR_FRAME";
            case ObjectType::DRAWER:                        return "DRAWER";
            case ObjectType::DRAWERS_FRAME:                 return "DRAWERS_FRAME";
            case ObjectType::GAME_OBJECT:                   return "GAME_OBJECT";
            case ObjectType::GENERIC_STATIC:                return "GENERIC_STATIC";
            case ObjectType::HEIGHT_MAP:                    return "HEIGHT_MAP";
            case ObjectType::PLANE:                         return "HOUSE_PLANE";
            case ObjectType::LIGHT:                         return "LIGHT";
            case ObjectType::OPENABLE:                      return "OPENABLE";
            case ObjectType::PICK_UP:                       return "PICK_UP";
            case ObjectType::PIANO:                         return "PIANO";
            case ObjectType::PIANO_KEY:                     return "PIANO_KEY";
            case ObjectType::PIANO_KEYBOARD_COVER:          return "PIANO_KEYBOARD_COVER";
            case ObjectType::PIANO_TOP_COVER:               return "PIANO_TOP_COVER";
            case ObjectType::PIANO_SHEET_MUSIC_REST:        return "PIANO_SHEET_MUSIC_REST";
            case ObjectType::PIANO_SHEET_SUSTAIN_PEDAL:     return "PIANO_SHEET_SUSTAIN_PEDAL";
            case ObjectType::PICTURE_FRAME:                 return "PICTURE_FRAME";
            case ObjectType::RAGDOLL_ENEMY:                 return "RAGDOLL_ENEMY";
            case ObjectType::RAGDOLL_V2:                    return "RAGDOLL_V2";
            case ObjectType::RAGDOLL_PLAYER:                return "RAGDOLL_PLAYER";
            case ObjectType::SHARK:                         return "SHARK";
            case ObjectType::SPAWN_POINT:                   return "SPAWN_POINT";
            case ObjectType::TOILET:                        return "TOILET";
            case ObjectType::TOILET_LID:                    return "TOILET_LID";
            case ObjectType::TOILET_SEAT:                   return "TOILET_SEAT";
            case ObjectType::TREE:                          return "TREE";
            case ObjectType::UNDEFINED:                     return "UNDEFINED";
            case ObjectType::WALL:                          return "WALL";
            case ObjectType::WALL_SEGMENT:                  return "WALL_SEGMENT";
            case ObjectType::WINDOW:                        return "WINDOW";
            default:                                        return "UNKNOWN";
        }
    }

    std::string PictureFrameTypeToString(PictureFrameType type) {
        switch (type) {
            case PictureFrameType::BIG_LANDSCAPE:       return "BIG_LANDSCAPE";
            case PictureFrameType::REGULAR_LANDSCAPE:   return "REGULAR_LANDSCAPE";
            case PictureFrameType::REGULAR_PORTRAIT:    return "REGULAR_PORTRAIT";
            case PictureFrameType::TALL_THIN:           return "TALL_THIN";
            default:                                    return "UNKNOWN";
        }
    }

    std::string PhysicsTypeToString(PhysicsType type) {
        switch (type) {
            case PhysicsType::NONE:                     return "NONE";
            case PhysicsType::CHARACTER_CONTROLLER:     return "CHARACTER_CONTROLLER";
            case PhysicsType::RIGID_DYNAMIC:            return "RIGID_DYNAMIC";
            case PhysicsType::RIGID_STATIC:             return "RIGID_STATIC";
            case PhysicsType::HEIGHT_FIELD:             return "HEIGHT_FIELD";
            case PhysicsType::GROUND_PLANE:             return "GROUND_PLANE";
            case PhysicsType::UNDEFINED:                return "UNDEFINED";
            default:                                    return "UNKNOWN";
        }
    }

    std::string WallTypeToString(WallType type) {
        switch (type) {
            case WallType::INTERIOR:        return "INTERIOR";
            case WallType::WEATHER_BOARDS:  return "WEATHER_BOARDS";
            case WallType::NONE:            return "NONE";
            default:                        return "UNKNOWN";
        }
    }

    std::string TrimTypeToString(TrimType type) {
        switch (type) {
            case TrimType::NONE:             return "NONE";
            case TrimType::PLASTER:          return "PLASTER";
            case TrimType::TIMBER:           return "TIMBER";
            default:                         return "UNKNOWN";
        }
    }

    DrawersType StringToDrawersType(const std::string& str) {
        if (str == "SMALL")     return DrawersType::SMALL;
        if (str == "LARGE")     return DrawersType::LARGE;
        return                  DrawersType::UNDEFINED;
    }

    BlendingMode StringToBlendingMode(const std::string& str) {
        if (str == "BLENDED") return BlendingMode::BLENDED;
        if (str == "ALPHA_DISCARDED") return BlendingMode::ALPHA_DISCARDED;
        if (str == "HAIR_UNDER_LAYER") return BlendingMode::HAIR_UNDER_LAYER;
        if (str == "HAIR_TOP_LAYER") return BlendingMode::HAIR_TOP_LAYER;
        if (str == "DO_NOT_RENDER") return BlendingMode::DO_NOT_RENDER;
        return BlendingMode::BLEND_DISABLED;
    }

    HouseType StringToHouseType(const std::string& str) {
        if (str == "SMALL_HOUSE")   return HouseType::SMALL_HOUSE;
        if (str == "MEDIUM_HOUSE")  return HouseType::MEDIUM_HOUSE;
        if (str == "LARGE_HOUSE")   return HouseType::LARGE_HOUSE;
        if (str == "NAMED")         return HouseType::NAMED;
        return HouseType::UNDEFINED;
    }

    std::string WeaponActionToString(const WeaponAction& weaponAction) {
        switch (weaponAction) {
            case WeaponAction::IDLE:                          return "IDLE";
            case WeaponAction::FIRE:                          return "FIRE";
            case WeaponAction::RELOAD:                        return "RELOAD";
            case WeaponAction::RELOAD_FROM_EMPTY:             return "RELOAD_FROM_EMPTY";
            case WeaponAction::DRAW_BEGIN:                    return "DRAW_BEGIN";
            case WeaponAction::DRAWING:                       return "DRAWING";
            case WeaponAction::DRAWING_FIRST:                 return "DRAWING_FIRST";
            case WeaponAction::DRAWING_WITH_SHOTGUN_PUMP:     return "DRAWING_WITH_SHOTGUN_PUMP";
            case WeaponAction::SPAWNING:                      return "SPAWNING";
            case WeaponAction::SHOTGUN_RELOAD_BEGIN:          return "SHOTGUN_RELOAD_BEGIN";
            case WeaponAction::SHOTGUN_RELOAD_SINGLE_SHELL:   return "SHOTGUN_RELOAD_SINGLE_SHELL";
            case WeaponAction::SHOTGUN_RELOAD_DOUBLE_SHELL:   return "SHOTGUN_RELOAD_DOUBLE_SHELL";
            case WeaponAction::SHOTGUN_RELOAD_END:            return "SHOTGUN_RELOAD_END";
            case WeaponAction::SHOTGUN_RELOAD_END_WITH_PUMP:  return "SHOTGUN_RELOAD_END_WITH_PUMP";
            case WeaponAction::SHOTGUN_MELEE:                 return "SHOTGUN_MELEE";
            case WeaponAction::ADS_IN:                        return "ADS_IN";
            case WeaponAction::ADS_OUT:                       return "ADS_OUT";
            case WeaponAction::ADS_IDLE:                      return "ADS_IDLE";
            case WeaponAction::ADS_FIRE:                      return "ADS_FIRE";
            case WeaponAction::MELEE:                         return "MELEE";
            case WeaponAction::TOGGLING_AUTO:                 return "TOGGLING_AUTO";
            case WeaponAction::DRY_FIRE:                      return "DRY_FIRE";
            default:                                          return "UNDEFINED";
        }
    }

    std::string ImageDataTypeToString(const ImageDataType& imageDataType) {
        switch (imageDataType) {
            case ImageDataType::UNCOMPRESSED: return "UNCOMPRESSED";
            case ImageDataType::COMPRESSED:   return "COMPRESSED";
            case ImageDataType::EXR:          return "EXR";
            default:                          return "UNDEFINED";
        }
    }
    std::string EditorModeToString(const EditorMode& editorMode) {
        switch (editorMode) {
            case EditorMode::HOUSE_EDITOR:      return "HOUSE_EDITOR";
            case EditorMode::MAP_OBJECT_EDITOR: return "MAP_OBJECT_EDITOR";
            case EditorMode::MAP_HEIGHT_EDITOR: return "MAP_HEIGHT_EDITOR";
            default:                            return "UNDEFINED";
        }
    }

    std::string DebugRenderModeToString(const DebugRenderMode& mode) {
        switch (mode) {
            case NONE:                                                  return "NONE";
            case ASTAR_MAP:                                             return "ASTAR_MAP";
            case DECALS:                                                return "DECALS";
            case PATHFINDING_RECAST:                                    return "PATHFINDING_RECAST";
            case PHYSX_ALL:                                             return "PHYSX_ALL";
            case PHYSX_RAYCAST:                                         return "PHYSX_RAYCAST";
            case PHYSX_COLLISION:                                       return "PHYSX_COLLISION";
            case RAYTRACE_LAND:                                         return "RAYTRACE_LAND";
            case PHYSX_EDITOR:                                          return "PHYSX_EDITOR";
            case BOUNDING_BOXES:                                        return "BOUNDING_BOXES";
            case RAGDOLLS:                                              return "RAGDOLLS";
            case RTX_LAND_AABBS:                                        return "RTX_LAND_AABBS";
            case RTX_LAND_TRIS:                                         return "RTX_LAND_TRIS";
            case RTX_LAND_TOP_LEVEL_ACCELERATION_STRUCTURE:             return "RTX_LAND_TOP_LEVEL_ACCELERATION_STRUCTURE";
            case RTX_LAND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES:         return "RTX_LAND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES";
            case RTX_LAND_TOP_AND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES: return "RTX_LAND_TOP_AND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES";
            case CLIPPING_CUBES:                                        return "CLIPPING_CUBES";
            case HOUSE_GEOMETRY:                                        return "HOUSE_GEOMETRY";
            case BONES:                                                 return "BONES";
            case BONE_TANGENTS:                                         return "BONE_TANGENTS";
            case DEBUG_LINE_MODE_COUNT:                                 return "DEBUG_LINE_MODE_COUNT";
            default: return "UNDEFINED";
        }
    }

    std::string ViewportModeToString(const ShadingMode& viewportMode) {
        switch (viewportMode) {
            case ShadingMode::SHADED:            return "SHADED";
            case ShadingMode::WIREFRAME:         return "WIREFRAME";
            case ShadingMode::WIREFRAME_OVERLAY: return "WIREFRAME_OVERLAY";
            default:                              return "UNDEFINED";
        }
    }

    std::string CameraViewToString(const CameraView& cameraView) {
        switch (cameraView) {
            case CameraView::PERSPECTIVE:   return "PERSPECTIVE";
            case CameraView::ORTHO:         return "ORTHOGRAPHIC";
            case CameraView::FRONT:         return "FRONT";
            case CameraView::BACK:          return "BACK";
            case CameraView::TOP:           return "TOP";
            case CameraView::BOTTOM:        return "BOTTOM";
            case CameraView::LEFT:          return "LEFT";
            case CameraView::RIGHT:         return "RIGHT";
            default:                        return "UNDEFINED";
        }
    }

    std::string EditorStateToString(const EditorState& editorState) {
        switch (editorState) {
            case EditorState::IDLE:                             return "IDLE";
            case EditorState::RESIZING_HORIZONTAL:              return "RESIZING_HORIZONTAL";
            case EditorState::RESIZING_VERTICAL:                return "RESIZING_VERTICAL";
            case EditorState::RESIZING_HORIZONTAL_VERTICAL:     return "RESIZING_HORIZONTAL_VERTICAL";
            case EditorState::GIZMO_TRANSLATING:                return "GIZMO_TRANSLATING";
            case EditorState::GIZMO_SCALING:                    return "GIZMO_SCALING";
            case EditorState::GIZMO_ROTATING:                   return "GIZMO_ROTATING";
            case EditorState::DRAGGING_SELECT_RECT:             return "DRAGGING_SELECT_RECT";
            case EditorState::PLACE_DOOR:                       return "PLACE_DOOR";
            case EditorState::PLACE_PICTURE_FRAME:              return "PLACE_PICTURE_FRAME";
            case EditorState::PLACE_TREE:                       return "PLACE_TREE";
            case EditorState::PLACE_WALL:                       return "PLACE_WALL";
            case EditorState::PLACE_WINDOW:                     return "PLACE_WINDOW";
            case EditorState::PLACE_PLAYER_CAMPAIGN_SPAWN:      return "PLACE_PLAYER_CAMPAIGN_SPAWN";
            case EditorState::PLACE_PLAYER_DEATHMATCH_SPAWN:    return "PLACE_PLAYER_DEATHMATCH_SPAWN";
            default:                                            return "UNDEFINED";
        }
    }

    std::string SharkMovementStateToString(const SharkMovementState& state) {
        switch (state) {
            case SharkMovementState::STOPPED:               return "STOPPED";
            case SharkMovementState::FOLLOWING_PATH:        return "FOLLOWING_PATH";
            case SharkMovementState::FOLLOWING_PATH_ANGRY:  return "FOLLOWING_PATH_ANGRY";
            case SharkMovementState::ARROW_KEYS:            return "ARROW_KEYS";
            case SharkMovementState::HUNT_PLAYER:           return "HUNT_PLAYER";
            default:                                        return "UNDEFINED";
        }
    }

    std::string SharkHuntingStateToString(const SharkHuntingState& state) {
        switch (state) {
            case SharkHuntingState::CHARGE_PLAYER:      return "CHARGE_PLAYER";
            case SharkHuntingState::BITING_PLAYER:      return "BITING_PLAYER";
            default:                        return "UNDEFINED";
        }
    }

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
