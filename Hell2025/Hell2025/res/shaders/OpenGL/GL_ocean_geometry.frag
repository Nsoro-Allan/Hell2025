#version 450
#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"
#include "../common/types.glsl"
#include "../common/util.glsl"

layout(location = 0) in vec3 WorldPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in highp vec3 DebugColor;

layout (binding = 0) uniform sampler2D DisplacementTexture_band0;
layout (binding = 1) uniform sampler2D NormalTexture_band0;
layout (binding = 2) uniform sampler2D DisplacementTexture_band1;
layout (binding = 3) uniform sampler2D NormalTexture_band1;
layout (binding = 4) uniform samplerCube cubeMap;
layout (binding = 5) uniform sampler2D GBufferWorldPositionTexture;
layout (binding = 6) uniform sampler2D FlashlightCookieTexture;
layout (binding = 7) uniform sampler2DArray FlashlighShadowMapArrayTexture;
layout (binding = 8) uniform sampler2D WaterDUDVNormalTexture;

layout (location = 0) out vec4 ColorOut;
layout (location = 1) out vec4 UnderwaterMaskOut;
layout (location = 2) out vec4 WorldPositionOut;

readonly restrict layout(std430, binding = 1) buffer rendererDataBuffer { RendererData  rendererData;   };
readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer { ViewportData  viewportData[]; };
readonly restrict layout(std430, binding = 4) buffer lightsBuffer       { Light         lights[];       };
readonly restrict layout(std430, binding = 5) buffer tileDataBuffer     { TileLightData tileData[];     };

uniform vec3 u_wireframeColor;
uniform vec3 u_viewPos;
uniform bool u_wireframe;
uniform int u_normalMultipler;
uniform float u_time;

//const vec3  WATER_ALBEDO = vec3(0.09, 0.12, 0.11);
const float WATER_METALLIC = 0.0;
const float WATER_ROUGHNESS = 0.02;

uniform vec3 u_fogColor = vec3(0.00326, 0.00217, 0.00073);
uniform float u_fogDensity = 0.05;

float computeFogFactorExp2(float dist) {
    return clamp(1.0 - exp(-u_fogDensity * u_fogDensity * dist * dist), 0.0, 1.0);
}

//uniform vec3 u_albedo = mix(WATER_ALBEDO * 0.075, vec3(0.01, 0.03, 0.04), 0.25);//vec3(0.01, 0.03, 0.04);
uniform vec3 u_albedo = WATER_ALBEDO;//vec3(0.01, 0.03, 0.04);
//uniform vec3 u_albedo = vec3(0.01, 0.03, 0.04);

uniform int u_mode;

const float u_nearMipDist = 50;   // distance at which LOD = 0
const float u_farMipDist = 100;   // distance at which LOD = max
const float u_maxMipLevel = 4.0;  // max mip count

uniform float u_oceanOriginY;

void main() {

    float fftResoltion_band0 = 512.0;
    float fftResoltion_band1 = 512.0;
    float patchSize_band0 = 8.0;
    float patchSize_band1 = 13.123;

    highp vec2 uv_band0 = fract(WorldPos.xz / patchSize_band0);
    highp vec2 uv_band1 = fract(WorldPos.xz / patchSize_band1);
    
    float displacementScale_band0 = patchSize_band0 / fftResoltion_band0;
    float displacementScale_band1 = patchSize_band1 / fftResoltion_band1;
    
    const float gridCellsPerWorldUnit_band0 = 1.0 / displacementScale_band0;
    const float gridCellsPerWorldUnit_band1 = 1.0 / displacementScale_band1;

    float viewDist = length(WorldPos - u_viewPos);
    float t = clamp((viewDist - u_nearMipDist) / (u_farMipDist - u_nearMipDist), 0.0, 1.0);
    float lod = t * u_maxMipLevel;
    //lod = clamp(lod, 1, uMaxMipLevel);


    // Estimate band 0 position
    vec2 bestGuessUV = uv_band0;
    float dispX = texture(DisplacementTexture_band0, bestGuessUV).x;
    float dispZ = texture(DisplacementTexture_band0, bestGuessUV).z;
    vec2 estimatedDisplacement = vec2(dispX, dispZ) / gridCellsPerWorldUnit_band0;
    vec2 estimatedWorldPosition = WorldPos.xz - estimatedDisplacement;
    vec2 estimatedUV = fract(estimatedWorldPosition / patchSize_band0);
    vec3 bestGuessNormal_band0 = textureLod(NormalTexture_band0, estimatedUV, lod).xyz;

    // Estimate band 0 position
    bestGuessUV = uv_band1;
    dispX = texture(DisplacementTexture_band1, bestGuessUV).x;
    dispZ = texture(DisplacementTexture_band1, bestGuessUV).z;
    estimatedDisplacement = vec2(dispX, dispZ) / gridCellsPerWorldUnit_band1;
    estimatedWorldPosition = WorldPos.xz - estimatedDisplacement;
    estimatedUV = fract(estimatedWorldPosition / patchSize_band1);
    //vec3 bestGuessNormal_band1 = texture(NormalTexture_band1, estimatedUV).xyz;
    vec3 bestGuessNormal_band1 = textureLod(NormalTexture_band1, estimatedUV, lod).xyz;    
    //vec3 bestGuessWorldPos_band1 = textureLod(DisplacementTexture_band1, estimatedUV, 0).xyz * displacementScale_band1;
    
    vec3 normal = normalize(mix (bestGuessNormal_band0, bestGuessNormal_band1, 0.5));

    if (!gl_FrontFacing) {
        normal *= - 1;
    }

   //float etimatedHeight = mix (bestGuessNormal_band0, bestGuessNormal_band1, 0.5));

    if (u_mode == 1) {
        normal = bestGuessNormal_band0;
    }
    if (u_mode == 2) {
        normal = bestGuessNormal_band1;
    }
    
    // Converge to up normal over distance
    float u_normalConvergeStartDist = 5;
    float u_normalConvergeMaxDist = 100;
    float u_normalConvergeMaxFactor = 1.0;
    float u_normalConvergeExponent = 0.5;
    float t2 = clamp((viewDist - u_normalConvergeStartDist) / (u_normalConvergeMaxDist - u_normalConvergeStartDist), 0.0, 1.0);
    t2 = pow(t2, u_normalConvergeExponent);
    t2 *= u_normalConvergeMaxFactor;
    normal = normalize(mix(normal, vec3(0.0, 1.0, 0.0), t2));

    
    const float metallic = 0.0;
    const float roughness = 0.1;
    const vec3 F0 = vec3(0.02); 

    vec3 surfaceLighting = vec3(0,0,0);

    // Moon light
    {
        vec3 moonColor = vec3(1.0, 0.9, 0.9);
        //vec3 moonLightDir = normalize(vec3(0.0, 0.2 , 0.5));
        vec3 moonLightDir = normalize(vec3(-0.5, 0.2, 0.0));
        vec3 L = normalize(moonLightDir);
        vec3 N = normalize(normal);
        vec3 V = normalize(u_viewPos - WorldPos);
        vec3 R = reflect(-V, N);
        float NoL = clamp(dot(N, L), 0.0, 1.0);

        // Direct Specular
        vec3 F_direct = fresnelSchlick(clamp(dot(N, V), 0.0, 1.0), F0);
        vec3 spec_direct = microfacetSpecular(L, V, N, F0, roughness);
        vec3 Lo_direct = spec_direct * moonColor * NoL;

        // Rotate R to match skybox -90 rotation
        float angle = radians(-90.0);
        float c = cos(angle);
        float s = sin(angle);
        mat3 rotateY = mat3(
             c, 0.0, s,
            0.0, 1.0, 0.0,
            -s, 0.0, c
        );
        vec3 R_rotated = rotateY * R;

        // IBL Reflection (specular-only)
        float damping_IBL = 0.5;
        vec3 F_reflection = fresnelSchlick(clamp(dot(N, V), 0.0, 1.0), F0);
        vec3 kS_IBL = F_reflection;
        vec3 reflection_IBL = texture(cubeMap, R_rotated).rgb * damping_IBL;;

        //kS_IBL *= kS_IBL; // HUGE HACK
        vec3 specular_IBL = reflection_IBL * kS_IBL;

        // IBL Diffuse
        vec3 irradiance = moonColor * 0.2;
        vec3 kD = (vec3(1.0) - kS_IBL) * (1.0 - metallic);
        vec3 diffuse_IBL = irradiance * WATER_ALBEDO;

        surfaceLighting += Lo_direct;          // direct moonlight spec
        surfaceLighting += diffuse_IBL;        // optional ambient diffuse
        surfaceLighting += specular_IBL;       // reflection-based spec

        // Subsurface scattering
        float h = WorldPos.y - u_oceanOriginY;
        float u_minHeight = u_oceanOriginY - 0.5;    // height at which radius=minR
        float u_maxHeight = u_oceanOriginY + 0.5;    // height at which radius=maxR
        float hNorm = clamp((h - u_minHeight) / (u_maxHeight - u_minHeight), 0.0, 1.0);
        vec3 sss_albedo = WATER_ALBEDO;
        float minR = 0.45;
        float maxR = 0.50;
        float sssFactor = 1.125;
        vec3 radius = vec3(mix(minR, maxR, hNorm));
        vec3 subColor = Saturate(sss_albedo.rgb, 1.5); // This is good
        float NdotL = max(dot(normalize(normal.xyz), normalize(L)), 0.0);
        vec3 sss = 0.2 * exp(-3.0 * abs(NdotL) / (radius + 0.001));
        vec3 sssColor = subColor * radius * sss * sssFactor;
        surfaceLighting += sssColor;
    }
    
    // Flash lights
    vec3 flashlightColor = vec3(0.9, 0.95, 1.1);
    vec3 flashlightLighting = vec3(0.0);

    for (int i = 0; i < 1; i++) {
        float flashlightModifer = viewportData[i].flashlightModifer;
        if (flashlightModifer > 0.05) {
            vec3 spotLightPos = viewportData[i].flashlightPosition.xyz;
            vec3 spotLightDir = normalize(viewportData[i].flashlightDir.xyz);
            vec3 flashlightViewPos = viewportData[i].inverseView[3].xyz;
            mat4 flashlightProjectionView = viewportData[i].flashlightProjectionView;

            // Core vectors
            vec3 L = normalize(spotLightPos - WorldPos);
            vec3 N = normalize(normal);
            vec3 V = normalize(flashlightViewPos - WorldPos);
            vec3 R = reflect(-V, N);
            float NoL = max(dot(N, L), 0.0);

            // Spotlight attenuation
            float dist = length(spotLightPos - WorldPos);
            float lightRadius = 50.0;
            float strength = 3.0;
            float innerAngle = cos(radians(5.0 * flashlightModifer));
            float outerAngle = cos(radians(25.0));
            float angleFactor = dot(L, -spotLightDir);
            float coneFalloff = smoothstep(outerAngle, innerAngle, angleFactor);
            float distanceFalloff = smoothstep(lightRadius, 0.0, dist);
            float spotAttenuation = coneFalloff * distanceFalloff * distanceFalloff * strength;

            // Shadow and cookie
            vec3 cookie = ApplyCookie(flashlightProjectionView, WorldPos, spotLightPos, flashlightColor, 10, FlashlightCookieTexture);

            // PBR direct lighting
            vec3 F_direct = fresnelSchlick(clamp(dot(N, V), 0.0, 1.0), F0);
            vec3 spec_direct = microfacetSpecular(L, V, N, F0, roughness);
            vec3 Lo_direct = spec_direct * flashlightColor * NoL;

            // Final flashlight lighting
            flashlightLighting += Lo_direct * spotAttenuation * cookie * flashlightModifer;

            // Optional: flashlight SSS
            float h = WorldPos.y - u_oceanOriginY;
            float u_minHeight = u_oceanOriginY - 0.5;    // height at which radius=minR
            float u_maxHeight = u_oceanOriginY + 0.5;    // height at which radius=maxR
            float hNorm = clamp((h - u_minHeight) / (u_maxHeight - u_minHeight), 0.0, 1.0);
            float minR = 0.45;
            float maxR = 0.50;
            float sssFactor = 1.0;
            vec3 radius = vec3(mix(minR, maxR, hNorm));
            vec3 subColor = Saturate(WATER_ALBEDO, 1.0);
            float NdotL_flash = max(dot(N, spotLightDir), 0.0);
            vec3 sss = 0.2 * exp(-3.0 * abs(NdotL_flash) / (radius + 0.001));
            vec3 sssColor = subColor * radius * sss * sssFactor;
            flashlightLighting += sssColor * flashlightModifer * coneFalloff;
        }
    }

    surfaceLighting += flashlightLighting;
    

    // Fog
    float u_fogStartDistance = 0.0;
    float u_fogEndDistance = 100.0;
    float u_fogExponent = 0.15;
    float dist = length(u_viewPos - WorldPos);
    float fogRange = u_fogEndDistance - u_fogStartDistance;
    float normDist = (dist - u_fogStartDistance) / max(fogRange, 0.0001);
    normDist = clamp(normDist, 0.0, 1.0);
    float fogEffect = pow(normDist, u_fogExponent);
    float fogFactor = 1.0 - fogEffect;
    surfaceLighting = mix(u_fogColor * 0.1, surfaceLighting, fogFactor);

    // Tonemap
    //color_linear = Tonemap_ACES(color_linear);
    //vec3 color_gamma = GammaCorrect(color_linear);
    //
    //// Output color
    //ColorOut = vec4(color_gamma, 1.0);
    ColorOut = vec4(surfaceLighting, 1.0);

    ColorOut.rgb = surfaceLighting;
    
    // wireframe override
    if (u_wireframe) {
        ColorOut.rgb = u_wireframeColor;
        ColorOut.a   = 1.0;
        ColorOut = vec4(DebugColor, 1.0);
        //ColorOut.rgb = u_wireframeColor;
    }

//    ColorOut =  vec4(normal, 1.0);
    
    //ColorOut.rgb = u_wireframeColor;

    // Gbuffer world position
    vec2 gBufferResolution = vec2(textureSize(GBufferWorldPositionTexture, 0));
    vec2 screenspace_uv = gl_FragCoord.xy / vec2(gBufferResolution);
    vec3 gBufferWorldPosition = texture(GBufferWorldPositionTexture, screenspace_uv).xyz;

    vec3 waterSurfacePosition = WorldPos;

    UnderwaterMaskOut.r = 1.0 - step(waterSurfacePosition.y, gBufferWorldPosition.y);
    //
    //if (gBufferWorldPosition.y > waterSurfacePosition.y) {    
    //    UnderwaterMaskOut.r = 0;
    //} 
    //else {    
    //    UnderwaterMaskOut.r = 1;
    //}

    // Write underwater mask value
    if (!gl_FrontFacing) {
        UnderwaterMaskOut.r = 0.5;
    }
    
    WorldPositionOut = vec4(WorldPos, 0);
}