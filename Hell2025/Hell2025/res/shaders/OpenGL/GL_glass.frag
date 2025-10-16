#version 460 core

#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"
#include "../common/types.glsl"

layout (location = 0) out vec4 FragOut;

layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;

layout (binding = 7) uniform sampler2D FlashlightCookieTexture;
layout (binding = 8) uniform sampler2DArray FlashlighShadowMapTextureArray;


readonly restrict layout(std430, binding = 1) buffer rendererDataBuffer { RendererData  rendererData;   };
readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer { ViewportData  viewportData[]; };
readonly restrict layout(std430, binding = 4) buffer lightsBuffer       { Light         lights[];       };
readonly restrict layout(std430, binding = 5) buffer tileDataBuffer     { TileLightData tileData[];     };

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;
uniform int u_viewportIndex;
uniform bool u_flipNormalMapY;
    

void main() {

    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;

    mat3 tbn = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    normalMap.rgb = normalMap.rgb * 2.0 - 1.0;
    normalMap = normalize(normalMap);

    if (u_flipNormalMapY) {
        normalMap.y *= -1;
    }

    vec3 normal = normalize(tbn * (normalMap));

    vec3 gammaBaseColor = pow(baseColor.rgb, vec3(2.2));
    float roughness = rma.r;
    float metallic = rma.g;

    //ivec2 tile = ivec2(gl_FragCoord.xy) / TILE_SIZE;
    //uint tileIndex = tile.y * rendererData.tileCountX + tile.x;
    //uint lightCount = tileData[tileIndex].lightCount;

    vec3 directLighting = vec3(0); 

    //for (uint i = 0; i < lightCount; ++i) {
    //    uint lightIndex = tileData[tileIndex].lightIndices[i];
    //    Light light = lights[lightIndex];

    for (uint i = 0; i < 4; ++i) {
        Light light = lights[i];

        vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
        vec3 lightColor =  vec3(light.colorR, light.colorG, light.colorB);
        float lightStrength = light.strength;
        float lightRadius = light.radius;
             
        directLighting += GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal.xyz, WorldPos.xyz, gammaBaseColor.rgb, roughness, metallic, ViewPos);
    }

    
    mat4 inverseProjection = viewportData[u_viewportIndex].inverseProjection;
    mat4 inverseView = viewportData[u_viewportIndex].inverseView;
    mat4 viewMatrix = viewportData[u_viewportIndex].view;
    vec3 viewPos = inverseView[3].xyz;    
    mat4 flashlightProjectionView = viewportData[u_viewportIndex].flashlightProjectionView;
    vec4 flashlightDir = viewportData[u_viewportIndex].flashlightDir;
    vec4 flashlightPosition = viewportData[u_viewportIndex].flashlightPosition;
    float flashlightModifer = viewportData[u_viewportIndex].flashlightModifer;

    if (flashlightModifer > 0.1) { 
        // Player flashlight
        int layerIndex = 0;
		vec3 forward = -normalize(vec3(inverseView[2].xyz));				
		vec3 spotLightPos = flashlightPosition.xyz;
		spotLightPos -= vec3(0, 0.0, 0);
		vec3 spotLightDir = normalize(spotLightPos - (viewPos - forward));
        spotLightDir = flashlightDir.xyz;
        spotLightDir.xyz = forward.xyz;
        vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
        float fresnelReflect = 0.9;
        float spotLightRadius = 50.0;
        float spotLightStregth = 3.0;        
        float innerAngle = cos(radians(0.0 * flashlightModifer));
        float outerAngle = cos(radians(30.0));         
        mat4 lightProjectionView = flashlightProjectionView;
        vec3 cookie = ApplyCookie(lightProjectionView, WorldPos.xyz, spotLightPos, spotLightColor, 10, FlashlightCookieTexture);
        vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, spotLightRadius, spotLightStregth, innerAngle, outerAngle, normal.xyz, WorldPos.xyz, gammaBaseColor.rgb, roughness, metallic, viewPos, lightProjectionView);
        vec4 FragPosLightSpace = lightProjectionView * vec4(WorldPos.xyz, 1.0);
        float shadow = SpotlightShadowCalculation(FragPosLightSpace, normal.xyz, spotLightDir, WorldPos.xyz, spotLightPos, viewPos, FlashlighShadowMapTextureArray, layerIndex);  
        spotLighting *= vec3(1 - shadow);
        spotLighting *= cookie * cookie * 5 * spotLightColor;
        directLighting += vec3(spotLighting) * flashlightModifer;
    }

    vec3 finalColor = directLighting;

    FragOut.rgb = vec3(finalColor);
	FragOut.a = 1.0;
}
