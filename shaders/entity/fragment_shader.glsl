#version 460 core

in vec4 vertexID;

out vec4 FragColor;


void main() {
	vec4 color = vec4(1.0, 0.3, 0.69, 1.0);
	FragColor = color;
}