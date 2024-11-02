#version 460 core

in vec4 vertexID;
in flat int out_light;

out vec4 FragColor;


void main() {
	vec4 color = vec4(1.0, 0.3, 0.69, 1.0);
	if (out_light == 0)
    	FragColor = color / 2;
	else
		FragColor = color;
}