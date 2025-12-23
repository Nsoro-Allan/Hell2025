#version 460 core
#include "../common/constants.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out vec4 WorldPosOut;
layout (location = 4) out vec4 EmissiveOut;

layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;
layout (binding = 3) uniform sampler2D DirtRoadBaseColorTexture;
layout (binding = 4) uniform sampler2D DirtRoadNormalTexture;
layout (binding = 5) uniform sampler2D DirtRoadRmaTexture;
layout (binding = 6) uniform sampler2D RoadMaskTexture;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec3 WorldPos;

uniform float u_worldWidth;
uniform float u_worldDepth;
uniform float u_discardHeight = 0.01;

void main() {
    if (WorldPos.y < u_discardHeight) {
        discard;
    }

    vec4 dirtBaseColor = texture2D(baseColorTexture, TexCoord);
    vec3 dirtNormalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 dirtRma = texture2D(rmaTexture, TexCoord).rgb;

    vec2 dirtRoadUV = TexCoord * 2.0;
    vec4 dirtRoadBaseColor = texture2D(DirtRoadBaseColorTexture, dirtRoadUV) + 0.1;
    vec3 dirtRoadNormalMap = texture2D(DirtRoadNormalTexture, dirtRoadUV).rgb;
    vec3 dirtRoadRma = texture2D(DirtRoadRmaTexture, dirtRoadUV).rgb;
    
    vec2 roadMaskWorldSize = textureSize(RoadMaskTexture, 0) * HEIGHTMAP_SCALE_XZ / 4;
    vec2 roadMaskUV = vec2(WorldPos.x / roadMaskWorldSize.x, WorldPos.z / roadMaskWorldSize.y);
    float roadMask = texture(RoadMaskTexture, roadMaskUV).r;

    vec3 baseColor = mix(dirtBaseColor.rgb, dirtRoadBaseColor.rgb, roadMask);
    vec3 normalMap = mix(dirtNormalMap.rgb, dirtRoadNormalMap.rgb, roadMask);
    vec3 rma = mix(dirtRma.rgb, dirtRoadRma.rgb, roadMask);

    //vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    mat3 tbn = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    normalMap.rgb = normalMap.rgb * 2.0 - 1.0;
    normalMap = normalize(normalMap);
    vec3 normal = normalize(tbn * (normalMap));

    BaseColorOut = vec4(baseColor, 1);
    RMAOut = vec4(rma, 1.0);
    NormalOut = vec4(normalize(normal), 0);
    WorldPosOut = vec4(WorldPos, 1.0);
    EmissiveOut = vec4(0.0, 0.0, 0.0, 0.45);
}
