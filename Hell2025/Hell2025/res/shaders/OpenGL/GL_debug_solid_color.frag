#version 460 core

layout (location = 0) out vec4 FragOut;
uniform vec3 u_color;

in vec4 WorldPos;
in vec2 TexCoords;

uniform bool u_isPath;

void main() {
    if (WorldPos.y < 30) {
        discard;
    }
    
    FragOut.rgb = u_color * 0.5;
    FragOut.rgb = vec3(TexCoords, 0);
	FragOut.a = 1.0;

    if (u_isPath) {    
        FragOut.rgb = vec3(1, 1, 1);
    }
    
        FragOut.rgb = vec3(1, 1, 1);
}
