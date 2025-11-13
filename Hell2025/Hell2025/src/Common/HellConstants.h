#pragma once

#define UNDEFINED_STRING "UNDEFINED_STRING"
#define DEFAULT_MATERIAL_NAME "CheckerBoard"
#define TIME_WRAP 10000.0f

#define NEAR_PLANE 0.005f
#define FAR_PLANE 256.0f

#define NO_ID 0

#define PINK                glm::vec4(0.98f, 0.06f, 0.75f, 1.0f)
#define ORANGE              glm::vec4(1.00f, 0.65f, 0.00f, 1.0f)
#define BLACK               glm::vec4(0.00f, 0.00f, 0.00f, 1.0f)
#define WHITE               glm::vec4(1.00f, 1.00f, 1.00f, 1.0f)
#define RED                 glm::vec4(1.00f, 0.00f, 0.00f, 1.0f)
#define GREEN               glm::vec4(0.00f, 1.00f, 0.00f, 1.0f)
#define BLUE                glm::vec4(0.00f, 0.00f, 1.00f, 1.0f)
#define YELLOW              glm::vec4(1.00f, 1.00f, 0.00f, 1.0f)
#define PURPLE              glm::vec4(1.00f, 0.00f, 1.00f, 1.0f)
#define GREY                glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)
#define LIGHT_BLUE          glm::vec4(0.00f, 1.00f, 1.00f, 1.0f)
#define LIGHT_GREEN         glm::vec4(0.16f, 0.78f, 0.23f, 1.0f)
#define LIGHT_RED           glm::vec4(0.80f, 0.05f, 0.05f, 1.0f)
#define TRANSPARENT         glm::vec4(0.00f, 0.00f, 0.00f, 0.0f)
#define GRID_COLOR          glm::vec4(0.50f, 0.50f, 0.60f, 1.0f)
#define OUTLINE_COLOR       glm::vec4(1.00f, 0.50f, 0.00f, 0.0f)
#define DEFAULT_LIGHT_COLOR glm::vec4(1.00f, 0.7799999713897705f, 0.5289999842643738f, 1.0f)

#define HELL_PRECISE_PI 3.14159265358979323846
#define HELL_PI 3.14159265f
#define HELL_EPSILON 1e-12f
#define HELL_SQRT_OF_TWO static_cast<float>(sqrt(2.0))
#define HELL_SQRT_OF_HALF 1.0f / HELL_SQRT_OF_TWO

#define VIEWPORT_INDEX_SHIFT 20 // Only 2 bits needed for 4 players

// Render limits
#define TEXTURE_ARRAY_SIZE 1024
#define MAX_INSTANCE_DATA_COUNT 8192
#define MAX_INDIRECT_DRAW_COMMAND_COUNT 4096
#define MAX_ANIMATED_TRANSFORMS 2048 // is this needed?
#define MAX_VIEWPORT_COUNT 4
#define SHADOW_CASCADE_COUNT 4
#define MAX_GPU_LIGHTS 512
#define MAX_SCREEN_SPACE_BLOOD_DECAL_COUNT 1024

#define DOOR_WIDTH 0.8f
#define DOOR_HEIGHT 2.0f
#define DOOR_DEPTH 0.034f

// System audio
#define AUDIO_SELECT "UI_Select.wav"

// Cursors
#define HELL_CURSOR_ARROW           0x00036001
#define HELL_CURSOR_IBEAM           0x00036002
#define HELL_CURSOR_CROSSHAIR       0x00036003
#define HELL_CURSOR_HAND            0x00036004
#define HELL_CURSOR_HRESIZE         0x00036005
#define HELL_CURSOR_VRESIZE         0x00036006

// Editor
#define EDITOR_LEFT_PANEL_WIDTH 500.0f

// Shadow maps
#define FLASHLIGHT_SHADOWMAP_SIZE 1024
#define SHADOW_MAP_HI_RES_SIZE 1024
#define SHADOW_MAP_CSM_SIZE 1024
#define SHADOW_NEAR_PLANE 0.05f
#define SHADOWMAP_HI_RES_COUNT 4

// Height map
#define HEIGHT_MAP_CHUNK_PIXEL_SIZE 32
#define HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE 8
#define HEIGHT_MAP_SIZE 256
#define HEIGHTMAP_SCALE_Y 40.0f
#define HEIGHTMAP_SCALE_XZ 0.25f
#define VERTICES_PER_CHUNK 1089 // 33 * 33
#define INDICES_PER_CHUNK 6144 // 32 * 32 * 6
#define CHUNK_COUNT_PER_MAP_CELL 8
#define MAP_CELL_WORLDSPACE_SIZE 64.0f

// Player stuff (move to player class)
#define PLAYER_CAPSULE_HEIGHT 0.4f
#define PLAYER_CAPSULE_RADIUS 0.15f

// Map limits
#define MAX_MAP_WIDTH 10
#define MAX_MAP_DEPTH 10

// Misc (delete me somehow)
#define SECTOR_SIZE_WORLD_SPACE (float(HEIGHT_MAP_SIZE) * float(HEIGHTMAP_SCALE_XZ))

// Tile Deferred
#define TILE_SIZE 24

// Normals
#define WORLD_UP glm::vec3(0.0f, 1.0f, 0.0f)

#define WOUND_MASK_TEXTURE_ARRAY_SIZE 25
#define WOUND_MASK_TEXTURE_SIZE 256
#define KANGAROO_MAX_HEALTH 400

// Global illumination
#define POINT_CLOUD_SPACING 0.4f

// Inventory
#define MAX_INVENTORY_X_SIZE 8
#define MAX_INVENTORY_Y_SIZE 4
