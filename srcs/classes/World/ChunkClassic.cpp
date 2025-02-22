#include <classes/World/ChunkClassic.hpp>


ChunkClassic::ChunkClassic(int x, int y, int z) : AChunk(x, y, z)
{
}

ChunkClassic::~ChunkClassic()
{
	// delete data;
}


//NORD SUD EST OUEST HAUT BAS
void ChunkClassic::createFace(int x, int y, int z, int tab_x, int tab_y, int tab_z, int dir)
{
	int start = vertices.size();
	switch (dir)
	{
		case 5 :
			if (tab_z > 0 && getData()[tab_z - 1 + tab_y * sizeZ + tab_x * sizeY * sizeZ] != 0)
				return ;
			vertices.push_back({glm::vec3(x, y, z), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y, z), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z), 0});
			vertices.push_back({glm::vec3(x, y + 1.0f, z), 0});
			break;
		case 4 :
			if (tab_z < sizeZ - 1  && getData()[tab_z + 1 + tab_y * sizeZ + tab_x * sizeY * sizeZ] != 0)
				return ;
			vertices.push_back({glm::vec3(x, y, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x + 1.0f, y, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x, y + 1.0f, z + 1.0f), 1});
			break;
		case 2 :
			if (tab_x < sizeX - 1 && getData()[tab_z + tab_y * sizeZ + (tab_x + 1) * sizeY * sizeZ] != 0)
				return ;
			vertices.push_back({glm::vec3(x + 1.0f, y, z), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y, z + 1.0f), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z), 0});
			break;
		case 3 :
			if (tab_x > 0 && getData()[tab_z + tab_y * sizeZ + (tab_x - 1) * sizeY * sizeZ] != 0)
				return ;
			vertices.push_back({glm::vec3(x, y, z), 1});
			vertices.push_back({glm::vec3(x, y, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x, y + 1.0f, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x, y + 1.0f, z), 1});
			break;
		case 0 :
			if (tab_y < sizeY - 1 && getData()[tab_z + (tab_y + 1) * sizeZ + tab_x * sizeY * sizeZ] != 0)
					return ;
			
			vertices.push_back({glm::vec3(x, y + 1.0f, z), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z), 0});
			vertices.push_back({glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), 0});
			vertices.push_back({glm::vec3(x, y + 1.0f, z + 1.0f), 0});
			break;
		case 1 :
			if (tab_y > 0 && getData()[tab_z + (tab_y - 1) * sizeZ + tab_x * sizeY * sizeZ] != 0)
				return ;
			
			vertices.push_back({glm::vec3(x, y, z), 1});
			vertices.push_back({glm::vec3(x + 1.0f, y, z), 1});
			vertices.push_back({glm::vec3(x + 1.0f, y, z + 1.0f), 1});
			vertices.push_back({glm::vec3(x, y, z + 1.0f), 1});
			break;
	}
	
	indices.push_back(start);
	indices.push_back(start + 1);
	indices.push_back(start + 2);
	indices.push_back(start + 3);
	indices.push_back(start + 2);
	indices.push_back(start);
}

void ChunkClassic::privGenerate(u_char *data)
{
	setData(data);
	// setIsGenerated(true);
}

void ChunkClassic::privCompile()
{
	u_char *blocks = getData();
	glm::ivec2 pos = getPos();
	int offset_x = pos.x * AChunk::sizeX;
	int offset_y = pos.y * AChunk::sizeY;
	int x = 1;
	int y = 1;
	int z = 1;
	for (int x = 0; x < sizeX; x++)	{
	for (int y = 0; y < sizeY; y++)	{
	for (int z = 0; z < sizeZ; z++)	{
		if (blocks[z + y * sizeZ + x * sizeY * sizeZ] != 0)
		{
				createFace(offset_x + x, offset_y + y, z, x, y, z, 0);
				createFace(offset_x + x, offset_y + y, z, x, y, z, 1);
				createFace(offset_x + x, offset_y + y, z, x, y, z, 2);
				createFace(offset_x + x, offset_y + y, z, x, y, z, 3);
				createFace(offset_x + x, offset_y + y, z, x, y, z, 4);
				createFace(offset_x + x, offset_y + y, z, x, y, z, 5);
		}
	}
	}
	}
	// setIsCompiled(true);
}

t_vbo_data ChunkClassic::privGetPtrVertices()
{
	t_vbo_data data;
	data.data = vertices.data();
	data.size = vertices.size() * sizeof(Vertex);
	return data;
}

std::vector<unsigned int>* ChunkClassic::privGetPtrIndices()
{
	return &indices;
}

bool ChunkClassic::privIsFilled(int x, int y, int z)
{
	std::cout << "Classic priv is filled" << std::endl;
	return getData()[z + y * sizeZ + x * sizeY * sizeZ] != 0;
}

u_char ChunkClassic::privBlockType(int x, int y, int z)
{
	return getData()[z + y * sizeZ + x * sizeY * sizeZ];
}