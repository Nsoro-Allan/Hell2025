#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) in vec3 inPosition;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

out vec3 TexCoords;
out vec4 WorldPos;

uniform mat4 u_modelMatrix;

void main() {
    int viewportIndex = gl_BaseInstance;
    mat4 projectionView = viewportData[viewportIndex].skyboxProjectionView;

    float angle = radians(-90.0);
    float c = cos(angle);
    float s = sin(angle);

    mat3 rotateY = mat3(
         c, 0.0, s,
        0.0, 1.0, 0.0,
        -s, 0.0, c
    );

    vec3 rotatedDirection = rotateY * inPosition;
    TexCoords = rotatedDirection;

    WorldPos = u_modelMatrix * vec4(inPosition, 1.0);
    gl_Position = projectionView * WorldPos;
}