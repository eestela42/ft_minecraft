#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textPos;
layout (location = 2) in float faceID;


out vec2 TexCoord;
out float out_face;

uniform mat4 matrix;
uniform float playerPosX;
uniform float playerPosY;
uniform float playerPosZ;

void main()
{
	// vertexID = gl_VertexID;
    // gl_Position = matrix * (vec4(pos.x, pos.y, pos.z, 1.0));
	TexCoord = textPos;
	out_face = faceID;
	// TexCoord = vec2(pos.x, pos.y);
    gl_Position = matrix * (vec4(pos.x, pos.y, pos.z, 1.0));
}