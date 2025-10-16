#version 460 core

layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;

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
    

  //   normal = texture2D(normalTexture, TexCoord).rgb;
   //  normal.y = 1 - normal.y;

    BaseColorOut = vec4(baseColor);
    NormalOut = vec4(normal, 1.0);   
    RMAOut = vec4(rma, 1.0);
    WorldPositionOut = vec4(WorldPos.rgb, 1.0);

    EmissiveOut = vec4(EmissiveColor, 0);
}
