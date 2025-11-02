#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#if ENABLE_BINDLESS == 1
    #extension GL_ARB_bindless_texture : enable        
    readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
	    uvec2 textureSamplers[];
    };    
    in flat int BaseColorTextureIndex;
    in flat int NormalTextureIndex;
    in flat int RMATextureIndex;
    in flat int WoundBaseColorTextureIndex;
    in flat int WoundNormalTextureIndex;
    in flat int WoundRMATextureIndex;

#else
    layout (binding = 0) uniform sampler2D baseColorTexture;
    layout (binding = 1) uniform sampler2D normalTexture;
    layout (binding = 2) uniform sampler2D rmaTexture;
    layout (binding = 3) uniform sampler2D woundBaseColorTexture;
    layout (binding = 4) uniform sampler2D woundNormalTexture;
    layout (binding = 5) uniform sampler2D woundRmaTexture;
#endif

layout (binding = 6) uniform sampler2DArray woundMaskTextureArray;
layout (binding = 7) uniform sampler2D WorldMirrorMaskTexture;

#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out vec4 WorldPositionOut;
layout (location = 4) out vec4 EmissiveOut;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;
in vec3 EmissiveColor;

in flat int WoundMaskTextureIndex;
in flat int BlockScreenSpaceBloodDecalsFlag;
in flat int EmissiveTextureIndex; 

uniform bool u_alphaDiscard;
uniform bool u_flipNormalMapY;

void main() {
    vec3 emissiveColor = EmissiveColor;

#if ENABLE_BINDLESS == 1
    vec4 baseColor = texture(sampler2D(textureSamplers[BaseColorTextureIndex]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[NormalTextureIndex]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[RMATextureIndex]), TexCoord).rgb;
    vec3 emissiveMap = texture(sampler2D(textureSamplers[EmissiveTextureIndex]), TexCoord).rgb;
#else
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
    vec3 emissiveMap = vec3(0, 0, 0); // This means no emissive colors in RenderDoc. Fix that!
#endif

    // Emissive
    if (EmissiveTextureIndex != -1) {
        emissiveColor = emissiveMap;
    }
    EmissiveOut = vec4(emissiveColor, 0);

    if (u_alphaDiscard) {
        if (baseColor.a < 0.5) {
            discard;
        }    
    }

    // Sensible defaults for wound texture
    vec4 woundBaseColor = vec4(0,0,0,0);
    vec3 woundNormalMap = vec3(0,0,0);
    vec3 woundRma = vec3(0,0,0);

    // If this mesh has a wound mask, then sample it
    float woundMask = 0;
    if (WoundMaskTextureIndex != -1) {
        #if ENABLE_BINDLESS == 1
            woundBaseColor = texture(sampler2D(textureSamplers[WoundBaseColorTextureIndex]), TexCoord);
            woundNormalMap = texture(sampler2D(textureSamplers[WoundNormalTextureIndex]), TexCoord).rgb;   
            woundRma = texture(sampler2D(textureSamplers[WoundRMATextureIndex]), TexCoord).rgb;
        #else
            woundBaseColor = texture2D(woundBaseColorTexture, TexCoord);
            woundNormalMap = texture2D(woundNormalTexture, TexCoord).rgb;
            woundRma = texture2D(woundRmaTexture, TexCoord).rgb;
        #endif  
        woundMask  = texture(woundMaskTextureArray, vec3(TexCoord, WoundMaskTextureIndex)).r;        

        // Hack to make the center of wounds black
        const float woundK = 0.1;
        const float woundDarkenStrength = 0.3;
        const float woundGamma = 0.1;
        woundMask = clamp(woundMask * 1.25, 0, 1);
        float t = clamp(pow(woundMask, woundGamma) * woundDarkenStrength, 0.0, 1.0);
        woundBaseColor.rgb = mix(woundBaseColor.rgb, vec3(0.0), t);
        woundRma.r = mix(woundRma.r, 0.0, t * 2);
        woundRma.b = mix(woundRma.b, 0.0, t * 2);
    }
    
    baseColor = mix(baseColor, woundBaseColor, woundMask);
    normalMap = mix(normalMap, woundNormalMap, woundMask);
    rma = mix(rma, woundRma, woundMask);

    mat3 tbn = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    normalMap.rgb = normalMap.rgb * 2.0 - 1.0;
    normalMap = normalize(normalMap);
    
    if (u_flipNormalMapY) {
        normalMap.y *= -1;
    }

    vec3 normal = normalize(tbn * (normalMap));

    BaseColorOut = vec4(baseColor);
    NormalOut = vec4(normal, 1.0);   

    RMAOut.rgb = rma;
    RMAOut.a = BlockScreenSpaceBloodDecalsFlag;

    WorldPositionOut = vec4(WorldPos.rgb, 1.0);
}
