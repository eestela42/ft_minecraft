#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 part_1;

// layout (location = 1) in vec4 part_1;
// layout (location = 2) in vec4 part_2;
// layout (location = 3) in vec4 part_3;
// layout (location = 4) in vec4 part_4;



uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
	mat4 matrix = proj * view * part_1;
    gl_Position = matrix * vec4(aPos, 1.0);
}