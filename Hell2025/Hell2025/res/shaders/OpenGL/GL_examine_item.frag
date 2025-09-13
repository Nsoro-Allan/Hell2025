#version 460 
#include "../common/lighting.glsl"
#include "../common/types.glsl"
#include "../common/post_processing.glsl"

layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;

layout (location = 0) out vec4 ColorOut;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 EmissiveColor;

uniform vec3 u_viewPos;

void main() {
    // Basecolor
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    baseColor.rgb = pow(baseColor.rgb, vec3(2.2)); // Gamma correct it
        
    // World normal
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    mat3 tbn = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    normalMap.rgb = normalMap.rgb * 2.0 - 1.0;
    normalMap = normalize(normalMap);
    vec3 normal = normalize(tbn * (normalMap));
    
    // Materials
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;   
    float roughness = rma.r;
    float metallic = rma.g;
    float ao = rma.b;

    vec3 directLighting = vec3(0.0);
    
    {
        vec3 lightPosition = vec3(0.5, 0.5, 2.5);
        vec3 lightColor = vec3(1.00, 0.7799999713897705, 0.5289999842643738);
        float lightRadius = 6.0;
        float lightStrength = 1.5;
        directLighting += GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal, WorldPos.xyz, baseColor.rgb, roughness, metallic, u_viewPos);
    }
    {
        vec3 lightPosition = vec3(0.25, 0.8, 1.5);
        vec3 lightColor = vec3(1.00, 0.7799999713897705, 0.5289999842643738);
        float lightRadius = 6.0;
        float lightStrength = 1.5;
        directLighting += GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal, WorldPos.xyz, baseColor.rgb, roughness, metallic, u_viewPos);
    }
    
    vec3 finalColor = directLighting.rgb;

    // Tone mapping
    finalColor = Tonemap_ACES(finalColor.rgb);   
        
    // Gamma correct
    finalColor = pow(finalColor, vec3(1.0/2.2));
    finalColor = clamp(finalColor, 0, 1);

    // Write it
    ColorOut = vec4(finalColor, 1.0);
}
