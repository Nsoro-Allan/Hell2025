#version 460 core

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out vec4 WorldPositionOut;
layout (location = 4) out vec4 EmissiveOut;

layout (binding = 2) uniform sampler2D NoiseTexture;

in vec3 Normal;
in vec3 WorldPos;

void main() {

    vec2 noiseUV = WorldPos.xz * 0.25;
    float noiseValue = texture(NoiseTexture, noiseUV).r;
    
    float noiseSq = noiseValue * noiseValue * 2.5;

    vec3 grassColor1 = vec3(0.22, 0.33, 0.18);  // Slightly desaturated green
    vec3 grassColor2 = vec3(0.45, 0.40, 0.12);  // More yellowish, dry grass

    vec3 grassColor3 = vec3(175.0, 166.0, 92.0)  / 255.0; // scorched ochre
    vec3 grassColor4 = vec3(124.0, 139.0, 74.0)  / 255.0; // shadowed sage
    vec3 grassColor5 = vec3(157.0, 177.0, 118.0) / 255.0; // fading emerald memory
    vec3 grassColor6 = vec3(165.0, 167.0, 100.0) / 255.0; // sun-bleached olive
    vec3 grassColor7 = vec3(183.0, 185.0, 110.0) / 255.0; // dry summer straw
    vec3 grassColor8 = vec3(142.0, 158.0, 94.0)  / 255.0; // cracked khaki earth
    vec3 grassColor9 = vec3(175.0, 200.0, 92.0)  / 255.0; 

    vec3 color = mix(grassColor2, grassColor9, noiseValue);
    color = mix(color, vec3(noiseSq), 0.3);
    
    BaseColorOut = vec4(color * 0.6, 1.0);
    RMAOut = vec4(0.9, 0.5, 1.0, 1.0);
    NormalOut = vec4(Normal, 0.0);
    WorldPositionOut = vec4(WorldPos, 1.0);
    EmissiveOut = vec4(0.0, 0.0, 0.0, 0.45);
}
