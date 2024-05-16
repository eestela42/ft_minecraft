#version 460 core

in vec2 TexCoord;
in float out_face;

out vec4 FragColor;
uniform sampler2D ourTexture;

void main() {
	// FragColor = vec4(0.05f * out_face, 0.1f * out_face, 0.15f * out_face, 1);
	FragColor = texture(ourTexture, TexCoord);
	// FragColor = vec4(0.2f * degrad, 0.2f * degrad, 0.8f, 1.0f);
}