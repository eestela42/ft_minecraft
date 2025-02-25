#version 460 core
out vec4 FragColor;

in float out_light;
in vec2 tex_out;
flat in int out_grass;
flat in int out_type;

uniform int TextureArraySize;

uniform sampler2DArray ourTextureArray;

void main()
{
	// if (out_grass == 1)
	// {
	// 	FragColor = texture(ourTextureArray, vec3(tex_out, out_type)) * vec4(0.4, 0.9, 0.6, 0) * out_light;
	// 	return;
	// }
	// FragColor = texture(ourTextureArray, vec3(tex_out, out_type))  * out_light;
}
