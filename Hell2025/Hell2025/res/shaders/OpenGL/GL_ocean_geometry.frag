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
uniform int u_mode;

uniform vec3 u_fogColor = vec3(0.00326, 0.00217, 0.00073);
uniform float u_fogDensity = 0.05;

uniform vec3 u_albedo = WATER_ALBEDO;
uniform float u_oceanOriginY;

const float WATER_METALLIC = 0.0;
const float WATER_ROUGHNESS = 0.02;

const float u_nearMipDist = 50.0;
const float u_farMipDist = 100.0;
const float u_maxMipLevel = 4.0;

// -90 degrees Y rotation (no cos/sin per fragment)
const mat3 kRotateYMinus90 = mat3(
    0.0, 0.0, -1.0,
    0.0, 1.0,  0.0,
    1.0, 0.0,  0.0
);

vec3 SampleEstimatedNormalBand(
    sampler2D displacementTex,
    sampler2D normalTex,
    vec2 worldXZ,
    float invPatchSize,
    float patchSize,
    float displacementScale,
    float lod
) {
    vec2 bestGuessUV = fract(worldXZ * invPatchSize);

    vec3 disp = texture(displacementTex, bestGuessUV).xyz;
    vec2 estimatedDisplacement = disp.xz * displacementScale;

    vec2 estimatedWorldPosition = worldXZ - estimatedDisplacement;
    vec2 estimatedUV = fract(estimatedWorldPosition / patchSize);

    return textureLod(normalTex, estimatedUV, lod).xyz;
}

void main() {

    // Band constants
    const float fftResoltion_band0 = 512.0;
    const float fftResoltion_band1 = 512.0;
    const float patchSize_band0 = 8.0;
    const float patchSize_band1 = 13.123;

    const float invPatchSize_band0 = 1.0 / patchSize_band0;
    const float invPatchSize_band1 = 1.0 / patchSize_band1;

    const float displacementScale_band0 = patchSize_band0 / fftResoltion_band0;
    const float displacementScale_band1 = patchSize_band1 / fftResoltion_band1;

    vec2 worldXZ = WorldPos.xz;

    float viewDist = length(WorldPos - u_viewPos);
    float t = clamp((viewDist - u_nearMipDist) / (u_farMipDist - u_nearMipDist), 0.0, 1.0);
    float lod = t * u_maxMipLevel;

    vec3 bestGuessNormal_band0 = SampleEstimatedNormalBand(
        DisplacementTexture_band0,
        NormalTexture_band0,
        worldXZ,
        invPatchSize_band0,
        patchSize_band0,
        displacementScale_band0,
        lod
    );

    vec3 bestGuessNormal_band1 = SampleEstimatedNormalBand(
        DisplacementTexture_band1,
        NormalTexture_band1,
        worldXZ,
        invPatchSize_band1,
        patchSize_band1,
        displacementScale_band1,
        lod
    );

    vec3 normal = normalize(mix(bestGuessNormal_band0, bestGuessNormal_band1, 0.5));

    if (!gl_FrontFacing) {
        //normal *= -1.0;
    }

    if (u_mode == 1) {
        normal = bestGuessNormal_band0;
    }
    if (u_mode == 2) {
        normal = bestGuessNormal_band1;
    }

    // Converge to up normal over distance
    const float u_normalConvergeStartDist = 0.0;
    const float u_normalConvergeMaxDist = 50.0;
    const float u_normalConvergeMaxFactor = 0.9;
    const float u_normalConvergeExponent = 0.5;

    float t2 = clamp((viewDist - u_normalConvergeStartDist) / (u_normalConvergeMaxDist - u_normalConvergeStartDist), 0.0, 1.0);
    t2 = pow(t2, u_normalConvergeExponent) * u_normalConvergeMaxFactor;
    normal = normalize(mix(normal, vec3(0.0, 1.0, 0.0), t2));

    vec3 N = normalize(normal);
    vec3 V_view = normalize(u_viewPos - WorldPos);

    if (dot(N, V_view) < 0.0) {
        V_view = -V_view;
    }

    const float metallic = 0.0;
    const float roughness = 0.1;
    const vec3 F0 = vec3(0.02);

    // Precompute SSS height terms once (used by moon + flashlight)
    float h = WorldPos.y - u_oceanOriginY;
    float u_minHeight = u_oceanOriginY - 0.5;
    float u_maxHeight = u_oceanOriginY + 0.5;
    float hNorm = clamp((h - u_minHeight) / (u_maxHeight - u_minHeight), 0.0, 1.0);

    float minR = 0.45;
    float maxR = 0.50;
    vec3 radius = vec3(mix(minR, maxR, hNorm));

    
    vec3 surfaceLighting = vec3(0.0);

    // Moon light (direct spec + IBL + SSS)
    {
        //vec3 moonColor = vec3(1.0, 0.9, 0.9);
        vec3 moonColor = GetMoonLightColor();
        vec3 L = vec3(-0.9284767, 0.3713907, 0.0); // pre-normalized

        if (!gl_FrontFacing) {
            L.x *= -1;
        }

        //if (!gl_FrontFacing) {
        //    L.y *= -1;
        //}

        float NoL = clamp(dot(N, L), 0.0, 1.0);

        vec3 spec_direct = microfacetSpecular(L, V_view, N, F0, roughness);
        vec3 Lo_direct = spec_direct * moonColor * NoL;

        vec3 R = reflect(-V_view, N);
        vec3 R_rotated = kRotateYMinus90 * R;

        float damping_IBL = 0.375;
        vec3 kS_IBL = fresnelSchlick(clamp(dot(N, V_view), 0.0, 1.0), F0);
        vec3 reflection_IBL = texture(cubeMap, R_rotated).rgb * damping_IBL;
        vec3 specular_IBL = reflection_IBL * kS_IBL;

        vec3 irradiance = moonColor * 1.0;
        vec3 diffuse_IBL = irradiance * WATER_ALBEDO * 0.075;
        
        if (!gl_FrontFacing) {
            diffuse_IBL *= 1;
            specular_IBL *= 2;
        }
            
        surfaceLighting += Lo_direct;
        surfaceLighting += diffuse_IBL;
        surfaceLighting += specular_IBL;


        // SSS
        vec3 sss_albedo = WATER_ALBEDO;
        float sssFactor = 2.5;
        vec3 subColor = Saturate(sss_albedo.rgb, 2.0);

        float NdotL = max(dot(N, L), 0.0);
        vec3 sss = 0.2 * exp(-3.0 * abs(NdotL) / (radius + 0.001));
        vec3 sssColor = subColor * radius * sss * sssFactor;

        surfaceLighting += sssColor;
    }

    // Flashlight
    for (int i = 0; i < 2; i++ ) {
        float flashlightModifer = viewportData[i].flashlightModifer;

        if (flashlightModifer > 0.05) {
            vec3 flashlightColor = vec3(0.9, 0.95, 1.1);

            vec3 spotLightPos = viewportData[i].flashlightPosition.xyz;
            vec3 spotLightDir = normalize(viewportData[i].flashlightDir.xyz);
            vec3 flashlightViewPos = viewportData[i].inverseView[3].xyz;
            mat4 flashlightProjectionView = viewportData[i].flashlightProjectionView;

            vec3 L = normalize(spotLightPos - WorldPos);
            vec3 V = normalize(flashlightViewPos - WorldPos);
            float NoL = max(dot(N, L), 0.0);

            float dist = length(spotLightPos - WorldPos);
            float lightRadius = 5.0;
            float strength = 3.0;

            float innerAngle = cos(radians(5.0 * flashlightModifer));
            float outerAngle = cos(radians(25.0));
            float angleFactor = dot(L, -spotLightDir);
            float coneFalloff = smoothstep(outerAngle, innerAngle, angleFactor);

            float distanceFalloff = smoothstep(lightRadius, 0.0, dist);
            float spotAttenuation = coneFalloff * distanceFalloff * distanceFalloff * strength;

            vec3 cookie = ApplyCookie(flashlightProjectionView, WorldPos, spotLightPos, flashlightColor, 10, FlashlightCookieTexture);

            vec3 spec_direct = microfacetSpecular(L, V, N, F0, roughness);
            vec3 Lo_direct = spec_direct * flashlightColor * NoL;

            vec3 flashlightLighting = Lo_direct * spotAttenuation * cookie * flashlightModifer;

            // Flashlight SSS
            vec3 subColor = Saturate(WATER_ALBEDO, 1.0);
            float NdotL_flash = max(dot(N, spotLightDir), 0.0);
            vec3 sss = 0.2 * exp(-3.0 * abs(NdotL_flash) / (radius + 0.01));
            vec3 sssColor = subColor * radius * sss * 1.5;

            flashlightLighting += sssColor * flashlightModifer * coneFalloff;

            surfaceLighting += flashlightLighting;
        }
    }

    // Fog (reuse viewDist)
    {
        float u_fogStartDistance = 0.0;
        float u_fogEndDistance = 100.0;
        float u_fogExponent = 0.1;

        float fogRange = u_fogEndDistance - u_fogStartDistance;
        float normDist = (viewDist - u_fogStartDistance) / max(fogRange, 0.0001);
        normDist = clamp(normDist, 0.0, 1.0);

        float fogEffect = pow(normDist, u_fogExponent);
        float fogFactor = 1.0 - fogEffect;

        surfaceLighting = mix(u_fogColor * 0.1, surfaceLighting, fogFactor);
    }

    ColorOut = vec4(surfaceLighting, 1.0);

    // Wireframe override (keep your final intent)
    if (u_wireframe) {
        ColorOut = vec4(DebugColor, 1.0);
    }

    // Underwater mask output
    UnderwaterMaskOut = vec4(0.0);

    vec2 gBufferResolution = vec2(textureSize(GBufferWorldPositionTexture, 0));
    vec2 screenspace_uv = gl_FragCoord.xy / gBufferResolution;
    vec3 gBufferWorldPosition = texture(GBufferWorldPositionTexture, screenspace_uv).xyz;

    UnderwaterMaskOut.r = 1.0 - step(WorldPos.y, gBufferWorldPosition.y);

    if (!gl_FrontFacing) {
        UnderwaterMaskOut.r = 0.5;
    }

    WorldPositionOut = vec4(WorldPos, 0.0);
}
