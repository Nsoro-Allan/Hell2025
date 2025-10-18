#version 460 core

#include "../common/util.glsl"
#include "../common/types.glsl"
#include "../common/constants.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

out vec2 TexCoord;
out vec4 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec3 EmissiveColor;

uniform int u_viewportIndex;
uniform mat4 u_model;

// temporarily here
uniform bool u_useMirrorMatrix;
uniform mat4 u_mirrorViewMatrix;
uniform vec3 u_mirrorNormal;
uniform vec4 u_mirrorClipPlane;

void main() {
    mat4 modelMatrix = u_model;
    mat4 inverseModelMatrix = inverse(modelMatrix);  
	mat4 projectionView = viewportData[u_viewportIndex].projectionView;            
    mat4 normalMatrix = transpose(inverseModelMatrix);
    
    WorldPos = modelMatrix * vec4(vPosition, 1.0);

    // Planar reflections
    if (u_useMirrorMatrix) {
        mat4 projection = viewportData[u_viewportIndex].projection;   
        projection[0][0] *= -1.0;      
        projectionView = projection * u_mirrorViewMatrix;

        gl_ClipDistance[0] = dot(WorldPos, u_mirrorClipPlane);
    }

    Normal = normalize(normalMatrix * vec4(vNormal, 0)).xyz;
    Tangent = normalize(normalMatrix * vec4(vTangent, 0)).xyz;
    BiTangent = normalize(cross(Normal, Tangent));
    EmissiveColor = vec3(0,0,0);
    
	TexCoord = vUV;
	gl_Position = projectionView * WorldPos;
}