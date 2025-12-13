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

    normalMap = mix(normalMap, vec3(0.5, 0.5, 1), 0.7);

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

    
    mat4 inverseProjection = viewportData[u_viewportIndex].inverseProjection;
    mat4 inverseView = viewportData[u_viewportIndex].inverseView;
    mat4 viewMatrix = viewportData[u_viewportIndex].view;
    vec3 viewPos = inverseView[3].xyz;    


    vec3 directLighting = vec3(0); 

    //for (uint i = 0; i < lightCount; ++i) {
    //    uint lightIndex = tileData[tileIndex].lightIndices[i];
    //    Light light = lights[lightIndex];

    for (uint i = 0; i < 8; ++i) {
        Light light = lights[i];

        vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
        vec3 lightColor =  vec3(light.colorR, light.colorG, light.colorB);
        float lightStrength = light.strength;
        float lightRadius = light.radius;
             
        directLighting += GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal.xyz, WorldPos.xyz, gammaBaseColor.rgb, roughness, metallic, ViewPos);
        
        vec3 toLight = lightPosition - WorldPos.xyz;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;
        vec3 viewDir = normalize(viewPos - WorldPos.xyz);
        float att = smoothstep(lightRadius, 0.0, dist) * lightStrength;
        directLighting += vec3(roughness * roughness * 0.01 * att) * lightColor;
    }

    
    mat4 flashlightProjectionView = viewportData[u_viewportIndex].flashlightProjectionView;
    vec4 flashlightDir = viewportData[u_viewportIndex].flashlightDir;
    vec4 flashlightPosition = viewportData[u_viewportIndex].flashlightPosition;
    float flashlightModifer = viewportData[u_viewportIndex].flashlightModifer;
    
    flashlightDir = viewportData[u_viewportIndex].cameraForward;
    flashlightPosition = viewportData[u_viewportIndex].viewPos;

    //if (flashlightModifer > 0.1) { 
    //    // Player flashlight
    //    int layerIndex = 0;
	//	vec3 forward = -normalize(vec3(inverseView[2].xyz));				
	//	vec3 spotLightPos = flashlightPosition.xyz;
	//	vec3 spotLightDir = flashlightDir.xyz;
    //    vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
    //    float fresnelReflect = 0.9;
    //    float spotLightRadius = 50.0;
    //    float spotLightStregth = 3.0;        
    //    float innerAngle = cos(radians(0.0 * flashlightModifer));
    //    float outerAngle = cos(radians(30.0));         
    //    mat4 lightProjectionView = flashlightProjectionView;
    //    vec3 cookie = ApplyCookie(lightProjectionView, WorldPos.xyz, spotLightPos, spotLightColor, 10, FlashlightCookieTexture);
    //    vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, spotLightRadius, spotLightStregth, innerAngle, outerAngle, normal.xyz, WorldPos.xyz, gammaBaseColor.rgb, roughness, metallic, viewPos, lightProjectionView);
    //    vec4 FragPosLightSpace = lightProjectionView * vec4(WorldPos.xyz, 1.0);
    //    float shadow = SpotlightShadowCalculation(FragPosLightSpace, normal.xyz, spotLightDir, WorldPos.xyz, spotLightPos, viewPos, FlashlighShadowMapTextureArray, layerIndex);  
    //    spotLighting *= vec3(1 - shadow);
    //    spotLighting *= cookie * cookie * 5 * spotLightColor;
    //    directLighting += vec3(spotLighting) * flashlightModifer;
    //}

    vec3 worldSpacePosition = WorldPos.xyz;
    float fragDistance = distance(viewPos, worldSpacePosition);

    for (int i = 0; i < 2; i++) {
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
            float spotLightRadius = 20.0;
            float spotLightStregth = 4.5;

            // EXPERIMENTAL NEW COLORS
            vec3 defaultLightColor = vec3(1.0, 0.7799999713897705, 0.5289999842643738);
            spotLightColor = mix(defaultLightColor, spotLightColor, 0.95);
            spotLightRadius = 20.0;
            spotLightStregth = 5.0;
            
            if (worldSpacePosition.y < 10) {
                spotLightStregth = 25;
            }

            float innerAngle = cos(radians(5.0 * flashlightModifer));
            float outerAngle = cos(radians(25.0));         
            mat4 lightProjectionView = flashlightProjectionView;
            vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, spotLightRadius, spotLightStregth, innerAngle, outerAngle, normal.xyz, worldSpacePosition.xyz, gammaBaseColor.rgb, roughness, metallic, flashlightViewPos, lightProjectionView);
            
            
            
            vec4 FragPosLightSpace = lightProjectionView * vec4(worldSpacePosition.xyz, 1.0);
            float shadow = SpotlightShadowCalculation(FragPosLightSpace, normal.xyz, spotLightDir, worldSpacePosition.xyz, spotLightPos, flashlightViewPos, FlashlighShadowMapTextureArray, layerIndex);  

            vec3 cookie = ApplyCookie(lightProjectionView, worldSpacePosition.xyz, spotLightPos, spotLightColor, spotLightRadius, FlashlightCookieTexture);

            float cookieStartDistance = 1.0;
            float cookieEndDistance = 10.0;
            float cookieDistanceExponent = 2;
            float cookieMinValue = 0.5;
            float cookieMaxValue = 5.0;
            float cookieDistScale;
            if(fragDistance <= cookieStartDistance) {
                cookieDistScale = cookieMinValue;
            } else if(fragDistance >= cookieEndDistance) {
                cookieDistScale = cookieMaxValue;
            } else {
                float t = (fragDistance - cookieStartDistance) / (cookieEndDistance - cookieStartDistance);
                cookieDistScale = mix(cookieMinValue, cookieMaxValue, pow(t, cookieDistanceExponent));
            }
            spotLighting *= cookieDistScale;
            //spotLighting *= spotLightColor;

            spotLighting *= vec3(1 - shadow);
            spotLighting *= cookie;
            directLighting += vec3(spotLighting) * flashlightModifer;


            vec3 toLight = flashlightPosition.xyz - WorldPos.xyz;
            float dist = length(toLight);
            vec3 lightDir = toLight / dist;
            vec3 viewDir = normalize(viewPos - WorldPos.xyz);
            float att = smoothstep(spotLightRadius, 0.0, dist) * spotLightStregth;
            directLighting += vec3(roughness * roughness * 0.02 * att) * spotLightColor * cookie;
        }
    }


    vec3 finalColor = directLighting;
    FragOut.rgb = vec3(finalColor);
	FragOut.a = 1.0;

    

    //finalColor.rgb = vec3(1,0,0);
    
}
