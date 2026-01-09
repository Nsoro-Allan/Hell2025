#define PI 3.14159265359
#define VIEWPORT_INDEX_SHIFT 20

#define HEIGHTMAP_SCALE_Y 40.0
#define HEIGHTMAP_SCALE_XZ 0.25
#define TILE_SIZE 24



//const vec3 WATER_ALBEDO = mix(vec3(0.4, 0.8, 0.6) * 0.1, vec3(0.01, 0.03, 0.04), 0.25);


//const vec3 WATER_ALBEDO = mix(vec3(0.04, 0.08, 0.06), vec3(0.01, 0.03, 0.04), 0.25);
const vec3 WATER_ALBEDO = vec3(0.0325, 0.0675, 0.0625) * 0.95; // same as above

const vec3 UNDER_WATER_TINT = mix(vec3(0.4, 0.8, 0.6) * 1.75, vec3(0.01, 0.03, 0.04), 0.25);


vec3 FOG_COLOR = vec3(0.222, 0.233, 0.27);