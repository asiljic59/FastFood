#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inCol;

out vec4 chCol;

uniform vec2 uPos;
uniform vec2 uScale;  // <--- add this

void main()
{
    vec2 scaled = inPos * uScale;   // scale around origin
    vec2 moved  = scaled + uPos;    // then move

    gl_Position = vec4(moved, 0.0, 1.0);
    chCol = inCol;
}
