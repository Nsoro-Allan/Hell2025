#pragma once
#include <cstdint>

enum class API {
    OPENGL,
    VULKAN,
    UNDEFINED
};

enum class WindowedMode { 
    WINDOWED, 
    FULLSCREEN 
};

enum struct Shortcut {
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    CTRL_A, CTRL_B, CTRL_C, CTRL_D, CTRL_E, CTRL_F, CTRL_G, CTRL_H, CTRL_I, CTRL_J,
    CTRL_K, CTRL_L, CTRL_M, CTRL_N, CTRL_O, CTRL_P, CTRL_Q, CTRL_R, CTRL_S, CTRL_T,
    CTRL_U, CTRL_V, CTRL_W, CTRL_X, CTRL_Y, CTRL_Z,
    ESC, NONE
};

enum class Alignment {
    CENTERED,
    CENTERED_HORIZONTAL,
    CENTERED_VERTICAL,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

enum class BakeState {
    AWAITING_BAKE,
    BAKING_IN_PROGRESS,
    BAKE_COMPLETE,
    UNDEFINED
};

enum class BlendingMode { 
    ALPHA_DISCARD,
    BLENDED,
    DEFAULT,
    HAIR_UNDER_LAYER,
    HAIR_TOP_LAYER,
    TOILET_WATER,
    MIRROR,
    GLASS,
    DO_NOT_RENDER,
    STAINED_GLASS,
    UNDEFINED
};

enum class ImageDataType {
    UNCOMPRESSED,
    COMPRESSED,
    EXR,
    UNDEFINED
};

enum class TextureWrapMode {
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    UNDEFINED
};

enum class TextureFilter {
    NEAREST,
    LINEAR,
    LINEAR_MIPMAP,
    UNDEFINED
};

enum class ObjectType : uint16_t {
    NO_TYPE = 0,
    CHARACTER_CONTROLLER,
    CHRISTMAS_LIGHTS,
    DECAL,
    DOOR,
    DOOR_FRAME,
    DRAWER,
    FIREPLACE,
    GAME_OBJECT,
    GENERIC_BOUNCABLE,
    GENERIC_STATIC,
    HEIGHT_MAP,
    HOUSE_PLANE,
    LADDER,
    LIGHT,
    MIRROR,
    PIANO,
    PICK_UP,
    PICTURE_FRAME,
    RAGDOLL_ENEMY,
    RAGDOLL_V2,
    RAGDOLL_PLAYER,
    SHARK,
    SPAWN_POINT,
    STAIRCASE,
    TREE,
    TRIM_SET,
    WALL,
    WALL_SEGMENT,
    WINDOW,

    ANIMATED_GAME_OBJECT,
    CHRISMAS_PRESENT,
    DOBERMANN,
    GENERIC_OBJECT,
    MESH_NODE,
    NAV_MESH,
    PLAYER,
    RAGDOLL_V1,
    TRIM,

    UNDEFINED
};

enum class Axis {
    X,
    Y,
    Z,
    NONE,
};

enum struct OpenAxis {
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    TRANSLATE_X_NEG,
    TRANSLATE_Y_NEG,
    TRANSLATE_Z_NEG,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    ROTATE_X_NEG,
    ROTATE_Y_NEG,
    ROTATE_Z_NEG,
};

enum class SplitscreenMode { 
    FULLSCREEN, 
    TWO_PLAYER, 
    FOUR_PLAYER, 
    SPLITSCREEN_MODE_COUNT 
};

enum class ShadingMode {
    SHADED,
    WIREFRAME,
    WIREFRAME_OVERLAY,
    SHADING_MODE_COUNT
};

enum class CameraView {
    PERSPECTIVE,
    ORTHO,
    FRONT,
    BACK,
    RIGHT,
    LEFT,
    TOP,
    BOTTOM,
    UNDEFINED
};

enum struct EditorSelectionMode {
    OBJECT,
    VERTEX
};

enum struct EditorState {
    IDLE,
    RESIZING_HORIZONTAL,
    RESIZING_VERTICAL,
    RESIZING_HORIZONTAL_VERTICAL,
    GIZMO_TRANSLATING,
    GIZMO_SCALING,
    GIZMO_ROTATING,
    DRAGGING_SELECT_RECT,

    PLACE_CHRISTMAS_LIGHTS,
    PLACE_OBJECT,
    PLACE_WALL,

    // Object placement REMOVEEEEEEE MEEEEEEEE
    PLACE_DOOR,
    PLACE_DRAWERS,
    PLACE_FLOOR,
    PLACE_HOUSE,
    PLACE_PICTURE_FRAME,
    PLACE_TREE,
    PLACE_WINDOW,
    PLACE_PLAYER_CAMPAIGN_SPAWN,
    PLACE_PLAYER_DEATHMATCH_SPAWN
};

enum WeaponAction {
    IDLE = 0,
    FIRE,
    DRY_FIRE,
    RELOAD,
    RELOAD_FROM_EMPTY,
    DRAW_BEGIN,
    DRAWING,
    DRAWING_FIRST,
    DRAWING_WITH_SHOTGUN_PUMP,
    SPAWNING,
    SHOTGUN_UNLOAD_BEGIN,
    SHOTGUN_UNLOAD_SINGLE_SHELL,
    SHOTGUN_UNLOAD_DOUBLE_SHELL,
    SHOTGUN_UNLOAD_END,
    SHOTGUN_RELOAD_BEGIN,
    SHOTGUN_RELOAD_SINGLE_SHELL,
    SHOTGUN_RELOAD_DOUBLE_SHELL,
    SHOTGUN_RELOAD_END,
    SHOTGUN_RELOAD_END_WITH_PUMP,
    SHOTGUN_MELEE,
    ADS_IN,
    ADS_OUT,
    ADS_IDLE,
    ADS_FIRE,
    MELEE,
    TOGGLING_AUTO,
    UNDEFINED
};

enum class ShellEjectionState {
    IDLE, AWAITING_SHELL_EJECTION
};

enum InputType {
    KEYBOARD_AND_MOUSE,
    CONTROLLER
};

enum CollisionGroup : uint64_t {
    NO_COLLISION = 0,
    BULLET_CASING = 1,
    CHARACTER_CONTROLLER = 2,
    ENVIROMENT_OBSTACLE = 4,
    GENERIC_BOUNCEABLE = 8,
    ITEM_PICK_UP = 16,
    RAGDOLL_PLAYER = 32,
    DOG_CHARACTER_CONTROLLER = 64,
    GENERTIC_INTERACTBLE = 128,
    ENVIROMENT_OBSTACLE_NO_DOG = 256,
    SHARK = 512,
    LADDER = 1024,
    RAGDOLL_ENEMY = 2048
};

// Re-evaluate how this works, coz it alway fucks you up, 
// and PhysX this group bitmask is used for more than just raycasts, pretty sure
enum RaycastGroup {
    RAYCAST_DISABLED = 0,
    RAYCAST_ENABLED = 1,
    DOBERMAN = 32
};

enum DebugRenderMode {
    NONE = 0, 
    ASTAR_MAP,
    DECALS,
    RAGDOLLS,
    PATHFINDING_RECAST,
    PHYSX_ALL,
    PHYSX_RAYCAST,
    PHYSX_COLLISION,
    RAYTRACE_LAND,
    PHYSX_EDITOR,
    BOUNDING_BOXES,
    RTX_LAND_AABBS,
    RTX_LAND_TRIS,
    RTX_LAND_TOP_LEVEL_ACCELERATION_STRUCTURE,
    RTX_LAND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES,
    RTX_LAND_TOP_AND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES,
    CLIPPING_CUBES,
    HOUSE_GEOMETRY,
    BONES,
    BONE_TANGENTS,
    BVH_CPU_PLAYER_RAYS,
    DEBUG_LINE_MODE_COUNT,
};

enum struct LightType {
    LAMP_POST = 0,
    HANGING_LIGHT,
    FIREPLACE_FIRE,
    UNDEFINED
};

enum struct EditorViewportSplitMode {
    SINGLE,
    FOUR_WAY_SPLIT,
    UNDEFINED
};

enum struct ItemType {
    HEAL,
    WEAPON,
    KEY,
    AMMO,
    USELESS,
    UNDEFINED
};

enum struct CollisionShapeType {
    BOX,
    CAPSULE,
    CONVEX_MESH,
    UNDEFINED
};

//enum struct PickUpTypeOld {
//    SHOTGUN_AMMO_BUCKSHOT,
//    SHOTGUN_AMMO_SLUG,
//    GLOCK,
//    GOLDEN_GLOCK,
//    AKS74U,
//    SPAS,
//    REMINGTON_870,
//    TOKAREV,
//    UNDEFINED
//};

enum struct EditorMode {
    HOUSE_EDITOR,
    MAP_HEIGHT_EDITOR,
    MAP_OBJECT_EDITOR,
    UNDEFINED,
};

enum struct PhysicsType {
    NONE = 0,
    RIGID_DYNAMIC,
    RIGID_STATIC,
    HEIGHT_FIELD,
    GROUND_PLANE,
    CHARACTER_CONTROLLER,
    UNDEFINED
};

enum struct OpeningState {
    CLOSED,
    CLOSING,
    OPEN,
    OPENING,
    UNDEFINED
};

enum struct DecalType {
    GLASS,
    PLASTER,
    UNDEFINED
};

enum struct TrimType {
    NONE,
    TIMBER,
    PLASTER,
    UNDEFINED
};

enum struct TrimSetType {
    FLOOR,
	MIDDLE,
	CEILING,
	CEILING_FANCY,
    UNDEFINED
};

enum struct HousePlaneType {
    FLOOR,
    CEILING,
    UNDEFINED
};

enum struct WallType {
    INTERIOR,
    WEATHER_BOARDS,
    UNDEFINED
};

enum struct TreeType {
    TREE_LARGE_0 = 0,
    TREE_LARGE_1,
    TREE_LARGE_2,
    BLACK_BERRIES,
    UNDEFINED
};

enum class RendererOverrideState {
    NONE = 0,
    BASE_COLOR,
    NORMALS,
    RMA,
    ROUGHNESS,
    METALIC,
    AO,
    CAMERA_NDOTL,
    TILE_HEATMAP,
    STATE_COUNT,
};

enum class OpenState {
    OPEN,
    OPENING,
    CLOSED,
    CLOSING,
    UNDEFINED
};

enum class PictureFrameType {
    BIG_LANDSCAPE,
    TALL_THIN,
    REGULAR_PORTRAIT,
    REGULAR_LANDSCAPE,
    UNDEFINED
}; 

enum class SharkMovementState { 
    STOPPED, 
    FOLLOWING_PATH, 
    FOLLOWING_PATH_ANGRY, 
    ARROW_KEYS, 
    HUNT_PLAYER,
    UNDEFINED
};

enum class SharkHuntingState { 
    CHARGE_PLAYER, 
    BITING_PLAYER, 
    UNDEFINED 
};

enum class SharkMovementDirection {
    STRAIGHT,
    LEFT,
    RIGHT,
    UNDEFINED
};

enum class RaycastIgnoreFlags : uint32_t {
    NONE = 0,
    PLAYER_CHARACTER_CONTROLLERS = 1 << 0,
    PLAYER_RAGDOLLS = 1 << 1,
};

enum class ChristmasPresentType : uint32_t {
    SMALL = 0,
    MEDIUM,
    LARGE,
    UNDEFINED
};


enum class GenericStaticType : uint32_t {
    COUCH = 0
};

enum class GenericBouncableType : uint32_t {
    COUCH_CUSHION_0 = 0,
    COUCH_CUSHION_1,
    COUCH_CUSHION_2,
    COUCH_CUSHION_3,
    COUCH_CUSHION_4
};

enum struct InventoryState {
    CLOSED,
    MAIN_SCREEN,
    EXAMINE_ITEM,
    MOVING_ITEM,
    ROTATING_ITEM,
    SHOP,
    UNDEFINED
};

enum class DebugTextMode{
    NONE,
    PER_PLAYER,
    GLOBAL,
    PROFILING,
    DEBUG_TEXT_MODE_COUNT
};

enum struct HouseType {
    SMALL_HOUSE,
    MEDIUM_HOUSE,
    LARGE_HOUSE,
    NAMED,
    UNDEFINED
};

enum struct FireplaceType {
	WOOD_STOVE,
	DEFAULT,
    UNDEFINED
};

enum struct GenericObjectType {
    CHRISTMAS_TREE,
    CHRISTMAS_PRESENT_SMALL,
    CHRISTMAS_PRESENT_LARGE,

    DRAWERS_SMALL,
    DRAWERS_LARGE,
    TOILET,
    COUCH,
    BATHROOM_BASIN,
    BATHROOM_CABINET,
    BATHROOM_TOWEL_RACK,
    
    MERMAID_ROCK,

    PLANT_BLACKBERRIES,
    PLANT_TREE,

    TEST_MODEL,
    TEST_MODEL2,
    TEST_MODEL3,
    TEST_MODEL4,
    UNDEFINED
};

enum struct DoorType {
    STANDARD_A,
    STANDARD_B,
    STAINED_GLASS,
    STAINED_GLASS2,
    UNDEFINED
};

enum struct DoorMaterialType {
    WHITE_PAINT,
    BACK_PAINT,
    RESIDENT_EVIL,
    UNDEFINED
};

enum struct ChainLinkType {
    DOOR_CHAIN,
    UNDEFINED
};

enum struct PhysicsShapeType {
    BOX,
    CONVEX_MESH
};

//enum struct MeshNodeType {
//    DEFAULT,
//    OPENABLE,
//    RIGID_STATIC,
//    RIGID_DYNAMIC
//};

inline RaycastIgnoreFlags operator|(RaycastIgnoreFlags a, RaycastIgnoreFlags b) {
    return static_cast<RaycastIgnoreFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline RaycastIgnoreFlags operator&(RaycastIgnoreFlags a, RaycastIgnoreFlags b) {
    return static_cast<RaycastIgnoreFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline RaycastIgnoreFlags& operator|=(RaycastIgnoreFlags& a, RaycastIgnoreFlags b) {
    a = a | b;
    return a;
}