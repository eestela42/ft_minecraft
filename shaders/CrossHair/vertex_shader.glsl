#version 460 core

layout (location = 0) in vec2 Pos;


void main()
{
    gl_Position = (vec4(Pos.x, Pos.y, 0.0, 1.0));
}