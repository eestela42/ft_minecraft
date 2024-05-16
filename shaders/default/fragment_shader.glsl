#version 460 core

in vec4 vertexID;

out vec4 FragColor;

void main() {
    FragColor = vertexID;
}