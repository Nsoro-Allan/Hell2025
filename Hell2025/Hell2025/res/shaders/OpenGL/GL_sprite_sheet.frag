#version 460 core
layout (location = 0) out vec4 FragOut;
layout (binding = 0) uniform sampler2D Texture;
in vec2 TexCoord;
in vec2 TexCoordNext;
in vec4 WorldPos;

uniform float u_mixFactor;

uniform vec4 u_worldBoundsMin;
uniform vec4 u_worldBoundsMax;
uniform bool u_useFireClipHeight;
void main() {

    if (u_useFireClipHeight && WorldPos.y > 31.62) discard;

    //if (WorldPos.y > u_worldBoundsMax.y) discard;

    vec4 color = texture2D(Texture, TexCoord);
    vec4 colorNext = texture2D(Texture, TexCoordNext);
    FragOut = mix(color, colorNext, u_mixFactor);
    //FragOut.rgb = pow(FragOut.rgb, vec3(2.2));
}
