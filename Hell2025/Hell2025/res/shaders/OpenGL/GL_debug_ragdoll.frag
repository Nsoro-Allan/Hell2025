#version 460 core

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalsOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out vec4 WorldPositionOut;
layout (location = 4) out vec4 EmissiveOut;
layout (location = 6) out vec4 EmssiveMask;

in vec3 Normal;
in vec4 WorldPos;

uniform int u_playerIndex;
uniform vec3 u_color;

void main() {

    BaseColorOut.rgb = u_color;
	BaseColorOut.a = 1.0;
    
    NormalsOut.rgb = normalize(Normal);
	NormalsOut.a = 1.0;//u_playerIndex;

    RMAOut.rgb = vec3(0.8, 0.1, 1);
	RMAOut.a = 0.0;

    EmssiveMask.rgb = vec3(0.0); // ??
	EmssiveMask.a = 1.0;         // ??

    EmissiveOut.rgb = vec3(0.0);
	EmissiveOut.a = 1.0;

    WorldPositionOut.rgb = WorldPos.rgb;
    WorldPositionOut.a = 1.0;
}
