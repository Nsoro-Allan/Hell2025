#include "types.glsl"

mat4 ToMat4(vec3 position, vec3 rotation, vec3 scale) {
    // Translation matrix
    mat4 translationMatrix = mat4(1.0);
    translationMatrix[3] = vec4(position, 1.0);

    // Rotation matrices (XYZ Euler Order)
    float cosX = cos(rotation.x), sinX = sin(rotation.x);
    float cosY = cos(rotation.y), sinY = sin(rotation.y);
    float cosZ = cos(rotation.z), sinZ = sin(rotation.z);

    mat4 rotX = mat4(
        1,  0,    0,   0,
        0,  cosX, -sinX, 0,
        0,  sinX, cosX, 0,
        0,  0,    0,   1
    );

    mat4 rotY = mat4(
        cosY,  0, sinY,  0,
        0,     1, 0,     0,
        -sinY, 0, cosY,  0,
        0,     0, 0,     1
    );

    mat4 rotZ = mat4(
        cosZ, -sinZ, 0, 0,
        sinZ, cosZ,  0, 0,
        0,    0,     1, 0,
        0,    0,     0, 1
    );

    // Combined rotation (Z * Y * X order)
    mat4 rotationMatrix = rotZ * rotY * rotX;

    // Scale matrix
    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = scale.x;
    scaleMatrix[1][1] = scale.y;
    scaleMatrix[2][2] = scale.z;

    // Final transformation matrix
    return translationMatrix * rotationMatrix * scaleMatrix;
}

float RandOLD(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float Rand(vec2 seed) {
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 WorldToScreen(vec3 worldPos, mat4 projView, vec2 viewportPosition, vec2 viewportSize) {
    vec4 clipSpace = projView * vec4(worldPos, 1.0);
    vec3 ndc = clipSpace.xyz / clipSpace.w; // Perspective divide

    // Convert from NDC (-1 to 1) to normalized screen UVs (0 to 1)
    vec2 screenUV = ndc.xy * 0.5 + 0.5;

    // 🔥 Ensure precise viewport size scaling
    return screenUV * viewportSize + viewportPosition;
}

float LinearizeDepth(float nonLinearDepth, float near, float far) {
    float z = nonLinearDepth * 2.0 - 1.0;  // Convert [0,1] range to [-1,1] (NDC space)
    return (2.0 * near * far) / (far + near - z * (far - near)); // Convert to linear depth
}

// Rotation matrix around the X axis.
mat3 RotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

// Rotation matrix around the Y axis
mat3 RotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

// Rotation matrix around the Z axis
mat3 RotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}

vec3 GetFlashLightColor() {      
    //vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
    vec3 flashLightColor = vec3(0.7, 0.75, 1.0);
    vec3 defaultLightColor = vec3(1.0, 0.7799999713897705, 0.5289999842643738);
    return mix(defaultLightColor, flashLightColor, 0.875) * 0.9;
}

vec3 GetMoonLightColor() {
    const vec3 UNDER_WATER_TINT = mix(vec3(0.4, 0.8, 0.6) * 1.75, vec3(0.01, 0.03, 0.04), 0.25); // sort out your includes then make this come from constants.glsl
    vec3 moonColor = vec3(1.0, 0.9, 0.9);
    vec3 moonColor2 = vec3(1, 0.7799999713897705, 0.5289999842643738);    
    moonColor = mix(moonColor, moonColor2, 0.5);
    moonColor = mix(moonColor, UNDER_WATER_TINT, 0.25);
    return moonColor;
}

// SplitscreenMode: 0 fullscreen, 1 two-player, 2 four-player
uint ComputeViewportIndexFromSplitscreenMode(ivec2 pixelCoords, ivec2 outputSize, int splitscreenMode) {
    int halfW = outputSize.x >> 1;
    int halfH = outputSize.y >> 1;
    
    if (splitscreenMode == 0) {
        return 0u;
    }
    
    uint iy = uint(pixelCoords.y < halfH);  // NOTE: flipped
    uint ix = uint(pixelCoords.x >= halfW);
    
    if (splitscreenMode == 1) {
        return iy;
    }
    
    // 4-player: 0 TL, 1 TR, 2 BL, 3 BR
    return ix + (iy << 1);

    //int halfW = outputSize.x >> 1;
    //int halfH = outputSize.y >> 1;
    //
    //uint ix = uint(pixelCoords.x >= halfW);
    //uint iy = uint(pixelCoords.y <  halfH);
    //
    //uint idx2 = iy;
    //uint idx4 = ix + (iy << 1);
    //
    //uint is2 = uint(splitscreenMode == 1);
    //uint is4 = uint(splitscreenMode == 2);
    //
    //return is2 * idx2 + is4 * idx4; // fullscreen returns 0
}

//vec2 GlobalPixelToViewportUV(ivec2 pixelCoords, vec2 viewportOffset, vec2 viewportSize) {
//    ivec2 localPx = pixelCoords - ivec2(viewportOffset.x, viewportOffset.y);
//    return (vec2(localPx) + 0.5) / vec2(float(viewportSize.x), float(viewportSize.y));
//}

vec2 GlobalPixelToViewportUV(ivec2 px, ViewportData v) {
    ivec2 localPx = px - ivec2(v.xOffset, v.yOffset);
    return (vec2(localPx) + 0.5) / vec2(float(v.width), float(v.height));
}

vec2 GlobalUVToViewportUV(vec2 globalUV, ViewportData v) {
    return (globalUV - vec2(v.posX, v.posY)) / vec2(v.sizeX, v.sizeY);
}

vec2 ViewportUVToGlobalUV(vec2 viewportUV, ViewportData v) {
    return vec2(v.posX, v.posY) + viewportUV * vec2(v.sizeX, v.sizeY);
}