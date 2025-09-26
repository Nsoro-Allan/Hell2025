#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
    ViewportData viewportData[];
};

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;

uniform mat4 modelMatrix;
uniform mat4 inverseModelMatrix;
uniform float u_textureScaling;

void main() {
    TexCoord = vUV * 50.0 * u_textureScaling;
    int viewportIndex = gl_BaseInstance;
    mat4 projectionView = viewportData[viewportIndex].projectionView;
    
    vec4 worldPos4 = modelMatrix * vec4(vPosition, 1.0);
    WorldPos = worldPos4.xyz;
    
    mat3 normalMatrix = transpose(mat3(inverseModelMatrix));
    Normal = normalize(normalMatrix * vNormal);
    Tangent = normalize(normalMatrix * vTangent);
    BiTangent = normalize(cross(Normal, Tangent));
    gl_Position = projectionView * worldPos4;
}
