#version 460 core

in vec4 vertexID;
in flat int out_light;

out vec4 FragColor;

uniform vec4 color;

void main() {
	if (out_light == 0)
    	FragColor = color / 2;
	else
		FragColor = color;
}