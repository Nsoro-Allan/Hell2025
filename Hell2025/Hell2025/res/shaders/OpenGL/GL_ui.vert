#version 460 core

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec4 vColor;

out vec4 Color;
out vec2 TexCoord;

uniform float u_clipMinX;
uniform float u_clipMinY;
uniform float u_clipMaxX;
uniform float u_clipMaxY;
uniform float u_renderTargetWidth;
uniform float u_renderTargetHeight;

void main() {
    Color = vColor;
    TexCoord = vUV;

    vec4 position = vec4(vPosition, 0.0, 1.0);

    float ndcLeft   = (u_clipMinX / u_renderTargetWidth)  * 2.0 - 1.0;
    float ndcRight  = (u_clipMaxX / u_renderTargetWidth)  * 2.0 - 1.0;
    float ndcTop    = 1.0 - (u_clipMinY / u_renderTargetHeight) * 2.0;
    float ndcBottom = 1.0 - (u_clipMaxY / u_renderTargetHeight) * 2.0;

    vec2 ndc = position.xy / position.w;

    gl_ClipDistance[0] = ndc.x - ndcLeft;    // left
    gl_ClipDistance[1] = ndcRight - ndc.x;   // right
    gl_ClipDistance[2] = ndc.y - ndcBottom;  // bottom
    gl_ClipDistance[3] = ndcTop - ndc.y;     // top

    gl_Position = position;
}