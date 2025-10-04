#version 460 core
#include "../common/types.glsl"
#include "../common/constants.glsl"

layout (location = 0) in vec3 vPosition;

layout(std430, binding = 3) readonly buffer renderItemsBuffer {
    RenderItem renderItems[];
};

out vec3 FragPos;

uniform mat4 shadowMatrices[6];
uniform int faceIndex;
uniform bool u_useInstanceData;
uniform mat4 u_modelMatrix;

void main() {

	vec4 worldPos = u_modelMatrix * vec4(vPosition, 1.0);

    if (u_useInstanceData) {
        int instanceOffset = gl_BaseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);
        int globalInstanceIndex = instanceOffset + gl_InstanceID;
    
        RenderItem renderItem = renderItems[globalInstanceIndex]; 
        mat4 modelMatrix = renderItem.modelMatrix;
        worldPos = modelMatrix * vec4(vPosition, 1.0);
    }

    FragPos = worldPos.xyz;
    gl_Position = shadowMatrices[faceIndex] * worldPos;
}