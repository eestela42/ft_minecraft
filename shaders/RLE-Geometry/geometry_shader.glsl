#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;


in GEO_OUT {
	int chunk_x;
	int chunk_y;
	int pos;
	int face;
	int type;
	int longX;
	int longY;
} geo_in[];


uniform float size_texture = 128;
uniform float width_Texture = 2048;

uniform int chunk_size_x;
uniform int chunk_size_y;


uniform mat4 matrix;
uniform vec4 cameraPos;

out vec2 tex_out;
flat out int out_grass;
out float 		out_light;
flat out int 	out_type;

void create_vertex(vec3 pos, vec2 texCoords, int type, int orientation)
{
	out_light = 1.0;
	if (orientation == 2 || orientation == 3)
		out_light = 0.8;
	gl_Position = matrix * vec4(pos, 1.0f);
	tex_out = texCoords;
	out_type = type;
	EmitVertex();
}

void create_face(vec3 pos, vec2 zero_texture, int face, int type, int sizeX, int sizeY)
{
	vec2 texCoords[4];


	texCoords[0] = vec2(0.0, 0.0);
	texCoords[1] = vec2(1.0 * sizeX, 0.0);
	texCoords[2] = vec2(1.0 * sizeX, 1.0 * sizeY);
	texCoords[3] = vec2(0.0, 1.0 * sizeY);

	switch (face)
	{
	case 0 : //back
			create_vertex(pos + vec3(0.0, 0.0, 1.0), texCoords[0], type, face);
			create_vertex(pos + vec3(1.0 * sizeX, 0.0, 1.0), texCoords[1], type, face);
			create_vertex(pos + vec3(0.0, 1.0 * sizeY, 1.0), texCoords[3], type, face);
			create_vertex(pos + vec3(1.0 * sizeX, 1.0 * sizeY, 1.0), texCoords[2], type, face);
		break;

	case  1 : //right
			create_vertex(pos + vec3(1.0, 0.0, 0.0), texCoords[0], type, face);
			create_vertex(pos + vec3(1.0, 0.0, 1.0 * sizeX), texCoords[1], type, face);
			create_vertex(pos + vec3(1.0, 1.0 * sizeY, 0.0), texCoords[3], type, face);
			create_vertex(pos + vec3(1.0, 1.0 * sizeY, 1.0 * sizeX), texCoords[2], type, face);
		break;

	case 2 : //front
			create_vertex(pos + vec3(0.0, 0.0, 0.0), texCoords[0], type, face);
			create_vertex(pos + vec3(1.0 * sizeX, 0.0, 0.0), texCoords[1], type, face);
			create_vertex(pos + vec3(0.0, 1.0 * sizeY, 0.0), texCoords[3], type, face);
			create_vertex(pos + vec3(1.0 * sizeX, 1.0 * sizeY, 0.0), texCoords[2], type, face);
		break;
	
	case 3 :  //left
			create_vertex(pos + vec3(0.0, 0.0, 0.0), texCoords[0], type, face);
			create_vertex(pos + vec3(0.0, 0.0, 1.0 * sizeX), texCoords[1], type, face);
			create_vertex(pos + vec3(0.0, 1.0 * sizeY, 0.0), texCoords[3], type, face);
			create_vertex(pos + vec3(0.0, 1.0 * sizeY, 1.0 * sizeX), texCoords[2], type, face);
		break;
	
	case  4 : //bot
		create_vertex(pos + vec3(0.0, 0.0, 0.0), texCoords[0], type, face);
		create_vertex(pos + vec3(1.0 * sizeX, 0.0, 0.0), texCoords[1], type, face);
		create_vertex(pos + vec3(0.0, 0.0, 1.0 * sizeY), texCoords[3], type, face);
		create_vertex(pos + vec3(1.0 * sizeX, 0.0, 1.0 * sizeY), texCoords[2], type, face);

		break;

	case  5 : //top
		create_vertex(pos + vec3(0.0, 1.0, 0.0), texCoords[0], type, face);
		create_vertex(pos + vec3(1.0 * sizeX, 1.0, 0.0), texCoords[1], type, face);
		create_vertex(pos + vec3(0.0, 1.0, 1.0 * sizeY), texCoords[3], type, face);
		create_vertex(pos + vec3(1.0 * sizeX, 1.0, 1.0 * sizeY), texCoords[2], type, face);
		break;
	}
	EndPrimitive();
}

void main() {
	int pos_block = geo_in[0].pos;
	int x = pos_block % chunk_size_x;
	int y = (pos_block % (chunk_size_x * chunk_size_y)) / chunk_size_y;
	int z = pos_block / (chunk_size_x * chunk_size_y);

	x += geo_in[0].chunk_x * chunk_size_x;
	y += geo_in[0].chunk_y * chunk_size_x;
	vec3 pos = vec3(x, z, y) - cameraPos.xyz;

	vec2 zero_texture = vec2(0.0, 0.0);
	int vtype = geo_in[0].type;
	zero_texture = vec2((vtype % 16 * 128) / width_Texture,
						1 - (vtype / 16 * 128) / width_Texture);

	out_light = 1.0;
	
	if (geo_in[0].face == 2 || geo_in[0].face == 3 || geo_in[0].face == 4)
			out_light = 0.8;
	
	out_grass = 0;

	if (vtype == 2 || vtype == 6)
		out_grass = 1;
	create_face(pos, zero_texture, geo_in[0].face, vtype, geo_in[0].longX, geo_in[0].longY);

}


	

	



	
	