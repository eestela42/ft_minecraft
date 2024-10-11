#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in int light;


out vec4 vertexID;
out flat int out_light;

uniform mat4 matrix;

void main()
{
	out_light = light;
	vertexID = vec4(0.2, 0.65 , 4.0, 1.0);
    gl_Position = matrix * vec4(aPos.xzy, 1.0);
}