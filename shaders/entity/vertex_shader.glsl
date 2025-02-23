#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 part_1;

// layout (location = 1) in vec4 part_1;
// layout (location = 2) in vec4 part_2;
// layout (location = 3) in vec4 part_3;
// layout (location = 4) in vec4 part_4;


out vec4 vertexID;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
	// mat4 myMat = mat4(part_1, part_2, part_3, part_4);
	mat4 matrix = proj * view * part_1;
	// mat4 matrix = model;
	vertexID = vec4(0.2, 0.65 , 4.0, 1.0);
    gl_Position = matrix * vec4(aPos, 1.0);
}