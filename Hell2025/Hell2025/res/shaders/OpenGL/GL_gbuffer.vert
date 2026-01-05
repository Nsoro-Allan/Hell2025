#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

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

layout(std430, binding = 3) readonly buffer renderItemsBuffer {
    RenderItem renderItems[];
};

out vec2 TexCoord;
out vec4 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec3 ViewPos;
out vec3 EmissiveColor;

#if ENABLE_BINDLESS
out flat int BaseColorTextureIndex;
out flat int NormalTextureIndex;
out flat int RMATextureIndex;
out flat int WoundBaseColorTextureIndex;
out flat int WoundNormalTextureIndex;
out flat int WoundRMATextureIndex;
#else
uniform int u_viewportIndex;
uniform int u_globalInstanceIndex;
#endif

out flat int EmissiveTextureIndex; // WARNING! this doens't work when bindless textures are disabled
out flat int WoundMaskTextureIndex;
out flat int BlockScreenSpaceBloodDecalsFlag;


// temporarily here
uniform bool u_useMirrorMatrix;
uniform mat4 u_mirrorViewMatrix;
uniform vec4 u_mirrorClipPlane;

void main() {

#if ENABLE_BINDLESS
    int viewportIndex = gl_BaseInstance >> VIEWPORT_INDEX_SHIFT;
    int instanceOffset = gl_BaseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);
    int globalInstanceIndex = instanceOffset + gl_InstanceID;
    
    BaseColorTextureIndex = renderItems[globalInstanceIndex].baseColorTextureIndex;
	NormalTextureIndex = renderItems[globalInstanceIndex].normalMapTextureIndex;
	RMATextureIndex = renderItems[globalInstanceIndex].rmaTextureIndex;   
    EmissiveTextureIndex = renderItems[globalInstanceIndex].emissiveTextureIndex;   
        
    WoundBaseColorTextureIndex = renderItems[globalInstanceIndex].woundBaseColorTextureIndex;
	WoundNormalTextureIndex = renderItems[globalInstanceIndex].woundNormalMapTextureIndex;
	WoundRMATextureIndex = renderItems[globalInstanceIndex].woundRmaTextureIndex;   

#else
    int globalInstanceIndex = u_globalInstanceIndex;
    int viewportIndex = u_viewportIndex;
    EmissiveTextureIndex = renderItems[globalInstanceIndex].emissiveTextureIndex; // required for some hackery
#endif

    RenderItem renderItem = renderItems[globalInstanceIndex]; 
    mat4 modelMatrix = renderItem.modelMatrix;
    mat4 inverseModelMatrix = renderItem.inverseModelMatrix;
	//mat4 projectionView = viewportData[viewportIndex].projectionView;   
	mat4 projection = viewportData[viewportIndex].projection;    
	mat4 view = viewportData[viewportIndex].view;
    mat4 normalMatrix = transpose(inverseModelMatrix);

    WoundMaskTextureIndex = renderItem.woundMaskTextureIndex;

    Normal = normalize(normalMatrix * vec4(vNormal, 0)).xyz;
    Tangent = normalize(normalMatrix * vec4(vTangent, 0)).xyz;
    BiTangent = normalize(cross(Normal, Tangent));
    
	TexCoord = vUV;
    ViewPos = viewportData[viewportIndex].inverseView[3].xyz;
    EmissiveColor = vec3(renderItems[globalInstanceIndex].emissiveR, renderItems[globalInstanceIndex].emissiveG, renderItems[globalInstanceIndex].emissiveB);

    // Absolute world position
    WorldPos = modelMatrix * vec4(vPosition, 1.0);
    
    // Planar reflections
    if (u_useMirrorMatrix) {  
        projection[0][0] *= -1.0;      
        view = u_mirrorViewMatrix;
        gl_ClipDistance[0] = dot(WorldPos, u_mirrorClipPlane);
    }
    
    // Old
    gl_Position = projection * view * WorldPos;
    
    // Camera relative position for depth precision
    //vec4 camRelativeWorldPos = vec4(WorldPos.xyz - ViewPos, 1.0);
    //gl_Position = projection * mat4(mat3(view)) * camRelativeWorldPos;

    BlockScreenSpaceBloodDecalsFlag = 0; // rethink this?
}