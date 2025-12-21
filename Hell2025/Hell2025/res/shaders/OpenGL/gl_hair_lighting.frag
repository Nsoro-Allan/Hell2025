#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#if ENABLE_BINDLESS
    #extension GL_ARB_bindless_texture : enable        
    readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
	    uvec2 textureSamplers[];
    };    
    in flat int BaseColorTextureIndex;
    in flat int NormalTextureIndex;
    in flat int RMATextureIndex;

#else
    layout (binding = 0) uniform sampler2D baseColorTexture;
    layout (binding = 1) uniform sampler2D normalTexture;
    layout (binding = 2) uniform sampler2D rmaTexture;
#endif

#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (location = 0) out vec4 FragOut;
layout (location = 1) out vec4 ViewSpaceDepthPreviousOut;
layout (binding = 3) uniform sampler2D ViewSpaceDepthTexture;
layout (binding = 4) uniform sampler2D FlashlightCookieTexture;

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
in mat4 FlashlightProjectionView;
in vec4 FlashlightDir;
in vec4 FlashlightPosition;
in float FlashlightModifer;
in vec3 CameraForward;

uniform float u_alphaBoost = 1.5;
uniform vec3 u_moonlightDir;

void main() {
#if ENABLE_BINDLESS
    vec4 baseColor = texture(sampler2D(textureSamplers[BaseColorTextureIndex]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[NormalTextureIndex]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[RMATextureIndex]), TexCoord).rgb;  
#else
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
#endif

	baseColor.rgb = pow(baseColor.rgb, vec3(2.2));
    float finalAlpha = baseColor.a * u_alphaBoost;

    mat3 tbn = mat3(Tangent, BiTangent, Normal);
    vec3 normal = normalize(tbn * (normalMap.rgb * 2.0 - 1.0));
    
    float roughness = rma.r;
    float metallic = rma.g;

    // Tiled lights
    ivec2 tile = ivec2(gl_FragCoord.xy) / TILE_SIZE;
    uint tileIndex  = uint(tile.y) * rendererData.tileCountX + uint(tile.x);
    uint lightCount = tileData[tileIndex].lightCount;

    vec3 directLighting = vec3(0.0);
    
    // Point lights
    for(uint i = 0; i < lightCount; ++i) {
        uint lightIndex = tileData[tileIndex].lightIndices[i];
        Light light = lights[lightIndex];
        vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
        vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);
        float shadow = 1;//ShadowCalculation(int(lightIndex), lightPosition, light.radius, WorldPos.xyz, ViewPos, normal, highResShadowCubeMapArray);
        directLighting += GetDirectLighting(lightPosition, lightColor, light.radius, light.strength, normal, WorldPos.xyz, baseColor.rgb, roughness, metallic, ViewPos) * shadow;
    }


    // Flashlights
   for (int i = 0; i < 4; i++) {
       float flashlightModifer = viewportData[i].flashlightModifer;
       if (flashlightModifer > 0.05) { 
           mat4 flashlightProjectionView = viewportData[i].flashlightProjectionView;
           vec4 flashlightDir = viewportData[i].flashlightDir;
           vec4 flashlightPosition = viewportData[i].flashlightPosition;
           vec3 flashlightViewPos = viewportData[i].inverseView[3].xyz;
           vec3 playerForward = -normalize(viewportData[i].inverseView[2].xyz);
           int layerIndex = i;			
		    vec3 spotLightPos = flashlightPosition.xyz;
           vec3 camightRight = normalize(viewportData[i].inverseView[0].xyz);
		    vec3 spotLightDir = flashlightDir.xyz;
           vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
           float fresnelReflect = 0.9;
           float spotLightRadius = 50.0;
           float spotLightStregth = 3.0;        
           float innerAngle = cos(radians(5.0 * flashlightModifer));
           float outerAngle = cos(radians(25.0));         
           mat4 lightProjectionView = flashlightProjectionView;
           vec3 cookie = ApplyCookie(lightProjectionView, WorldPos.xyz, spotLightPos, spotLightColor, 10, FlashlightCookieTexture);
           vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, spotLightRadius, spotLightStregth, innerAngle, outerAngle, normal.xyz, WorldPos.xyz, baseColor.rgb, roughness, metallic, flashlightViewPos, lightProjectionView);
           vec4 FragPosLightSpace = lightProjectionView * vec4(WorldPos.xyz, 1.0);
           float shadow = 0;//SpotlightShadowCalculation(FragPosLightSpace, normal.xyz, spotLightDir, WorldPos.xyz, spotLightPos, flashlightViewPos, FlashlighShadowMapArrayTexture, layerIndex);  
   
           spotLighting *= vec3(1 - shadow);
           spotLighting *= cookie *  spotLightColor;
           directLighting += vec3(spotLighting) * flashlightModifer;
       }
   }
   
    vec3 moonColor = vec3(1.0, 0.9, 0.9);
    moonColor = vec3(1, 0.7799999713897705, 0.5289999842643738);
    float moonLightStrength = 0.05;
    vec3 moonLighting = GetDirectionalLighting(u_moonlightDir, moonColor, moonLightStrength, normal.xyz, WorldPos.xyz, baseColor.rgb, roughness, metallic, ViewPos);
    
    // Ambient light
    vec3 amibentLightColor = vec3(1, 0.98, 0.94);
    float ambientIntensity = 0.0005;
    vec3 ambientColor = baseColor.rgb * amibentLightColor;
    vec3 ambientLighting = ambientColor * ambientIntensity;
	float factor = min(1, 1 - metallic * 1.0); // Ambient hack
	ambientLighting *= (1.0) * vec3(factor);
    
    vec3 finalColor = directLighting.rgb + moonLighting + ambientLighting;
    
    finalColor.rgb = finalColor.rgb * finalAlpha;
    FragOut = vec4(finalColor, finalAlpha * 1.0);

    vec2 uv_screenspace = gl_FragCoord.xy / vec2(rendererData.hairBufferWidth, rendererData.hairBufferHeight);
    float ViewSpaceDepth = texture2D(ViewSpaceDepthTexture, uv_screenspace).r;
    ViewSpaceDepthPreviousOut = vec4(ViewSpaceDepth, 0, 0, 0);
}
