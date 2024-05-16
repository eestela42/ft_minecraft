#version 460 core
layout (location = 0) in int chunk_x; 
layout (location = 1) in int chunk_y; 
layout (location = 2) in int pos; 
layout (location = 3) in int face; 
layout (location = 4) in int type;
layout (location = 5) in int light;

out vec2 TexCoord;
out float out_light;
out flat int out_grass;


uniform float size_texture = 128;
uniform float width_Texture = 2048;

uniform int chunk_size_x;
uniform int chunk_size_y;

uniform vec4 cameraPos;

uniform mat4 matrix;


void main()
{
	
	int point = 0;
	point = gl_VertexID % 4;


	vec2 zero_texture = vec2(0.0, 0.0);
	int vtype = type;
	out_grass = 0;
	if (type == 19 && face == 5)
	{
		vtype = 16;
		out_grass = 1;
	}

	if (type == 30)
		out_grass = 2;

	float tmp = ((vtype - 1) * size_texture) / width_Texture;
	zero_texture = vec2((vtype % 16 * 128) / width_Texture,
						1 - (vtype / 16 * 128) / width_Texture);

	vec2 texCoords[4];
	
	vec3 points[4];


	switch (face)
	{
	case 0 :

		points[0] = vec3(0.0, 0.0, 1.0);
		points[1] = vec3(1.0, 0.0, 1.0);
		points[2] = vec3(1.0, 1.0, 1.0);
		points[3] =  vec3(0.0, 1.0, 1.0);

		
		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + size_texture / width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture / width_Texture);
		break;
	case  1 :
		points[0] = vec3(1.0, 0.0, 0.0);
		points[1] = vec3(1.0, 0.0, 1.0);
		points[2] = vec3(1.0, 1.0, 1.0);
		points[3] =  vec3(1.0, 1.0, 0.0);

		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + size_texture / width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture / width_Texture);
		break;
	case 2 :
		points[0] = vec3(0.0, 0.0, 0.0);
		points[1] = vec3(1.0, 0.0, 0.0);
		points[2] = vec3(1.0, 1.0, 0.0);
		points[3] =  vec3(0.0, 1.0, 0.0);

		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture/ width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture/ width_Texture, zero_texture.y + size_texture/ width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture/ width_Texture);
		break;
	
	case 3 :
		points[0] = vec3(0.0, 0.0, 0.0);
		points[1] = vec3(0.0, 0.0, 1.0);
		points[2] = vec3(0.0, 1.0, 1.0);
		points[3] = vec3(0.0, 1.0, 0.0);

		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + size_texture / width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture / width_Texture);
		break;
	
	case  4 :
		points[0] = vec3(0.0, 0.0, 0.0);
		points[1] = vec3(1.0, 0.0, 0.0);
		points[2] = vec3(1.0, 0.0, 1.0);
		points[3] =  vec3(0.0, 0.0, 1.0);

		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + size_texture / width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture / width_Texture);
		break;
	case  5 :
		points[0] = vec3(0.0, 1.0, 0.0);
		points[1] = vec3(1.0, 1.0, 0.0);
		points[2] = vec3(1.0, 1.0, 1.0);
		points[3] =  vec3(0.0, 1.0, 1.0);

		texCoords[0] = vec2(zero_texture.x + 0.0, zero_texture.y + 0.0);
		texCoords[1] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + 0.0);
		texCoords[2] = vec2(zero_texture.x + size_texture / width_Texture, zero_texture.y + size_texture / width_Texture);
		texCoords[3] = vec2(zero_texture.x + 0.0, zero_texture.y + size_texture / width_Texture);
		
		break;
	}

    vec3 newPos;

	int pos_block = pos;
	int x = pos_block % chunk_size_x;
	int y = (pos_block % (chunk_size_x * chunk_size_y)) / chunk_size_y;
	int z = pos_block / (chunk_size_x * chunk_size_y);

	x += chunk_x * chunk_size_x;
	y += chunk_y * chunk_size_x;

	newPos.x = x + points[point].x;
	newPos.y = z + points[point].y;
	newPos.z = y + points[point].z;

	newPos -= cameraPos.xyz;
	

	gl_Position = matrix * vec4(newPos, 1.0);
	
	TexCoord = texCoords[point];
	out_light = 0.1 * light;
	// ourColor = vec3(0.0, 1.0, 1.0);
}
