#version 460 core
layout (location = 0) in int chunk_x; 
layout (location = 1) in int chunk_y; 
layout (location = 2) in int pos; 
layout (location = 3) in int data;
// layout (location = 2) in int pos; 
// layout (location = 3) in int face; 
// layout (location = 4) in int type;
// layout (location = 5) in int longX;
// layout (location = 6) in int longY;




uniform float size_texture = 128;
uniform float width_Texture = 2048;

uniform int chunk_size_x;
uniform int chunk_size_y;

uniform mat4 matrix;

out GEO_OUT {
	int chunk_x;
	int chunk_y;
	int pos;
	int face;
	int type;
	int longX;
	int longY;
} geo_out;

out int tmp_vertID;

void main()
{
	tmp_vertID = gl_VertexID;

	geo_out.chunk_x = chunk_x;
	geo_out.chunk_y = chunk_y;
	geo_out.pos = pos;

	geo_out.longY = data & 0xFF;
	geo_out.longX = data >> 8 & 0xFF;
	geo_out.type = data >> 16 & 0xFF;
	geo_out.face = data >> 24 & 0xFF;

	// geo_out.pos = pos;
	// geo_out.face = face;
	// geo_out.type = type;
	// geo_out.longX = longX;
	// geo_out.longY = longY;
	
}
