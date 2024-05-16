#version 460 core

layout (location = 0) in vec3 aPos;

out vec4 vertexID;

uniform mat4 matrix;

void main()
{
	vertexID = vec4(float(gl_VertexID % 128) / 256, float(gl_VertexID % (128 * 128) / 256) / 256 , float(gl_VertexID / (128 * 128)) / 256, 1);
    gl_Position = matrix * vec4(aPos.x, aPos.z, aPos.y, 1.0);
}