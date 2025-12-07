#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform vec2 uPos;
uniform vec2 uScale;

out vec2 vUV;

void main()
{
    vec2 scaled = aPos * uScale;
    vec2 moved = scaled + uPos;
    gl_Position = vec4(moved, 0.0, 1.0);
    vUV = aUV;
}

