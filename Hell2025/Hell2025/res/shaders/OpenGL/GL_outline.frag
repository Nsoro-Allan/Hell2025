#version 460 core
#include "../common/types.glsl"

layout (location = 0) out vec4 FragOut;
layout(r8, binding = 0) uniform image2D outlineMask;
layout(binding = 1) uniform sampler2D outlineMaskTexture;

uniform int u_offsetCount;
uniform int u_viewportIndex;
uniform vec2 u_offsets[256];

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

flat in int offsetIndex;

void main2() {
    ivec2 centerCoords = ivec2(gl_FragCoord.xy);
    vec2 offsetCoords = centerCoords + u_offsets[offsetIndex];

    int xOffset = viewportData[u_viewportIndex].xOffset;
    int yOffset = viewportData[u_viewportIndex].yOffset;
    int viewportWidth = viewportData[u_viewportIndex].width;
    int viewportHeight = viewportData[u_viewportIndex].height;
    
    offsetCoords.x = clamp(offsetCoords.x, xOffset, xOffset + viewportWidth - 1);
    offsetCoords.y = clamp(offsetCoords.y, yOffset, yOffset + viewportHeight - 1);
    
    float centerMask = imageLoad(outlineMask, centerCoords).r;
    float offsetMask = imageLoad(outlineMask, ivec2(offsetCoords)).r;
    
    if (centerMask == offsetMask) {
        discard;
    }
    else {
        FragOut = vec4(offsetMask, 0.0, 0.0, 0.0);
    }
}

void main() {
    // Get the size of the texture to convert pixel coords to UVs
    ivec2 texSize = textureSize(outlineMaskTexture, 0);
    vec2 invTexSize = 1.0 / vec2(texSize);

    // Center coordinate in UV space [0.0, 1.0]
    vec2 centerUV = gl_FragCoord.xy * invTexSize;
    
    // Offset coordinate in pixel space, then convert to UV space
    vec2 offsetCoords = gl_FragCoord.xy + u_offsets[offsetIndex];
    vec2 offsetUV = offsetCoords * invTexSize;

    // Sample both locations using floating-point UVs
    float centerMask = texture(outlineMaskTexture, centerUV).r;
    float offsetMask = texture(outlineMaskTexture, offsetUV).r;

    if (centerMask == offsetMask) {
        discard;
    }
    else {
        FragOut = vec4(offsetMask, 0.0, 0.0, 0.0);
    }
}