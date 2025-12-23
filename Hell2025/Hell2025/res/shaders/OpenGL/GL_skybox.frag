#version 460
#include "../common/types.glsl"

layout (location = 0) out vec4 FinalLightingOut;
layout (location = 1) out vec4 WorldPositionOut;

layout (binding = 0) uniform samplerCube cubeMap;

in vec3 TexCoords;
in vec4 WorldPos;

in flat int ViewportIndex;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

vec3 GetWorldRay(vec2 fragCoordWindow, int viewportIndex) {
    vec2 viewportOrigin = vec2(viewportData[viewportIndex].xOffset, viewportData[viewportIndex].yOffset);
    vec2 viewportSize = vec2(viewportData[viewportIndex].width, viewportData[viewportIndex].height);
    vec2 fragCoord = fragCoordWindow - viewportOrigin;
    vec2 ndc = (fragCoord / viewportSize) * 2.0 - 1.0;
    mat4 inverseProjectionView = viewportData[viewportIndex].inverseProjectionView;

    // If you ever feed in top-left coords somewhere, flip ndc.y here
    // ndc.y = -ndc.y;

    vec4 nearH = inverseProjectionView * vec4(ndc, -1.0, 1.0);
    vec4 farH  = inverseProjectionView * vec4(ndc,  1.0, 1.0);
    vec3 nearW = nearH.xyz / nearH.w;
    vec3 farW  = farH.xyz  / farH.w;

    return normalize(farW - nearW);
}

void main() {
    vec3 skyColor = texture(cubeMap, TexCoords).rgb;
    vec3 skyLinear = pow(skyColor, vec3(2.6));

    vec3 viewPos = viewportData[ViewportIndex].viewPos.xyz;
    mat4 inverseProjectionView = viewportData[ViewportIndex].inverseProjectionView;
    vec2 resolution = vec2(viewportData[ViewportIndex].width, viewportData[ViewportIndex].height);

    FinalLightingOut = vec4(skyLinear, 1.0);

    float u_cutoffWorldY = viewPos.y;//0.1;
    vec3 u_belowColorLinear = vec3(1,0,0);  

    // World ray under water fog
    vec3 horizonColor = vec3(0.6, 0.2, 0.6);
    vec3 downColor = vec3(0.0);

    vec3 rayDir = GetWorldRay(gl_FragCoord.xy, ViewportIndex);
    float amount = 0.0125;
    float curve = 0.5; // higher = stays bright longer, lower = darkens faster
    float downwardness = clamp(dot(rayDir, vec3(0.0, -1.0, 0.0)), 0.0, 1.0);
    float t = pow(downwardness, curve); // 0 at horizon, 1 when looking down
    vec3 finalRayFog = mix(horizonColor, downColor, t) * amount;
        
    vec3 outColor = skyLinear;
    float cutoffWorldY = viewPos.y;

    if (WorldPos.y < cutoffWorldY) {
        float fadeDistance = 50.0;   // world meters to reach full fog color
        float fadeExponent = 0.9;   // >1 slower near cutoff, <1 faster

        float depthBelow = cutoffWorldY - WorldPos.y;          // 0 at cutoff, increases downward
        float f = clamp(depthBelow / fadeDistance, 0.0, 1.0);  // 0..1
        f = pow(f, fadeExponent);

        // At cutoff: f=0 so black. Deeper: f->1 so finalRayFog.
        outColor = finalRayFog * f;
    }

    FinalLightingOut = vec4(outColor, 1.0);

    WorldPositionOut = vec4(WorldPos.rgb * vec3(10, 1, 10), 1.0);
}
