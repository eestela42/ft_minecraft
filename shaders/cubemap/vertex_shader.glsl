#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 matrix;

void main()
{
	// mat4 new_matrix = mat4(mat3(matrix));
	
    TexCoords = vec3(aPos.x, -aPos.y, aPos.z);
    gl_Position = matrix * vec4(aPos, 1.0);
}