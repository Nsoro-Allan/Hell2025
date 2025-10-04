#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 Normal;
out vec4 WorldPos;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    Normal = normalize(normalMatrix * vNormal);

    WorldPos = u_model * vec4(vPos, 1.0);

	gl_Position = u_projection * u_view * WorldPos;
}