#version 450

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#if ENABLE_BINDLESS == 1
    #extension GL_ARB_bindless_texture : enable
#endif

layout (location = 0) out vec4 DecalMaskOut;
layout(binding = 0) uniform sampler2D GBufferRMATexture;
layout(binding = 1) uniform sampler2D WorldPositionTexture;
layout(binding = 2) uniform sampler2D GBufferNormalTexture;

#if ENABLE_BINDLESS == 1
    readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
        uvec2 textureSamplers[];
    };
#else
    layout(binding = 3) uniform sampler2D DecalTex0;
    layout(binding = 4) uniform sampler2D DecalTex1;
    layout(binding = 5) uniform sampler2D DecalTex2;
    layout(binding = 6) uniform sampler2D DecalTex3;
#endif

struct ScreenSpaceDecal {
    mat4 modelMatrix;
    mat4 inverseModelMatrix;
    int type;
    int textureIndex;
    int padding1;
    int padding2;
};
readonly restrict layout(std430, binding = 12) buffer ScreenSpaceDecalBuffer {
    ScreenSpaceDecal screenSpaceDecals[];
};

uniform int u_decalCount;

void main() {
    ivec2 pixelCoords = ivec2(gl_FragCoord.xy);
    ivec2 size = textureSize(WorldPositionTexture, 0);
    vec2  uv = gl_FragCoord.xy / vec2(size);

    // Do nothing on walls
    vec3 normal      = texture(GBufferNormalTexture, uv).rgb;
    float angleToFloor = abs(normal.y);
    float floorFade    = step(0.5, angleToFloor);
    if (floorFade <= 0.0) {
        return;
    }

    // Blocking mask
    vec4 rma = texture(GBufferRMATexture, uv);
    if (rma.a == 0.0) {
       return;
    }

    vec3 worldPos  = texture(WorldPositionTexture, uv).rgb;

    float bestMask = 0.0;
    for (int i = 0; i < u_decalCount; ++i) {
        vec4 objPos = screenSpaceDecals[i].inverseModelMatrix * vec4(worldPos, 1.0);
        vec3 dist   = vec3(0.5) - abs(objPos.xyz);

        // Multiply by a factor to steepen the falloff (tweak 1000 for higher/sharper, lower for softer)
        float fx = clamp(dist.x * 1000.0, 0.0, 1.0);
        float fy = clamp(dist.y * 1000.0, 0.0, 1.0);
        float fz = clamp(dist.z * 1000.0, 0.0, 1.0);
        float cubeFade = fx * fy * fz;

        // Skip if completely outside
        if (cubeFade <= 0.0) continue;

        int textureIndex = screenSpaceDecals[i].textureIndex;
        vec2 texCoords = clamp(objPos.xz + 0.5, 0.0, 1.0);

        
        #if ENABLE_BINDLESS == 1
        vec4 texData = texture(sampler2D(textureSamplers[textureIndex]), texCoords);
        float a = texData.a;

        bestMask = max(bestMask, a);

        // Break if this pixel has already reached a full decal max of 1.0
        if (bestMask >= 1.0) break;
        #else

        #endif
    }

    DecalMaskOut = vec4(bestMask, bestMask, bestMask, 0);
}
