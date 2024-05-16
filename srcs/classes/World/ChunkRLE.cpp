#include <classes/World/ChunkRLE.hpp>
#include <map>
#include <classes/World/PerlinNoise.hpp>
#include <iostream>
#include <cstring>

ChunkRLE::~ChunkRLE()
{
	delete[] this->rubans_id;
	delete this->rubans;
	this->data = NULL;
}

ChunkRLE::ChunkRLE(int posX, int posY) : Chunk(posX, posY)
{
	this->data = NULL;
	this->rubans = NULL;
	this->rubans_id = new u_int[Chunk::sizeZ];
}

bool ChunkRLE::isFilled(int x, int y, int z)
{
	if (!IsGenerated())
	{
		return false;
	}
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;
	while (tmp_z <= z)
	{
		tmp_z += this->data[pos + 1];
		pos += 2;
	}
	if (this->data[pos - 2] != 0)
		return true;
	return false;
}

u_char ChunkRLE::blockType(int x, int y, int z)
{
	if (!IsGenerated())
	{
		return 0;
	}
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;
	while (tmp_z <= z)
	{
		tmp_z += this->data[pos + 1];
		pos += 2;
	}
	return this->data[pos - 2];
}

std::vector<u_char> *ChunkRLE::GetAdjacentRuban(int x, int y, int z, int &pos, u_char direction)
{

	ChunkRLE *neighbours;

	switch (direction) // pos du neighbour
	{
	case 0: // north
	{
		if (y == sizeY - 1)
		{ // find north neighbour
			neighbours = (ChunkRLE *)(neighborChunks[0]);

			if (!neighbours || (neighbours->posY != this->posY + 1 && neighbours->posX == this->posX))
				return (NULL);
			pos = neighbours->GetRubanPos(x, 0, z);
			return (neighbours->rubans);
		}
		pos = GetRubanPos(x, y + 1, z);
		break;
	}
	case 1: // east
	{
		if (x == sizeX - 1) // find east neighbour
		{
			neighbours = (ChunkRLE *)(neighborChunks[1]);
			if (!neighbours || (neighbours->posX != this->posX + 1 && neighbours->posY == this->posY))
				return (NULL);
			pos = neighbours->GetRubanPos(0, y, z);
			return (neighbours->rubans);
		}
		pos = GetRubanPos(x + 1, y, z);
		break;
	}
	case 2: // south
	{
		if (y == 0)
		{ // find south neighbour
			neighbours = (ChunkRLE *)(neighborChunks[2]);
			if (!neighbours || (neighbours->posY != this->posY - 1 && neighbours->posX == this->posX))
				return (NULL);
			pos = neighbours->GetRubanPos(x, sizeY - 1, z);
			return (neighbours->rubans);
		}
		pos = GetRubanPos(x, y - 1, z);
		break;
	}
	case 3: // west
	{
		if (x == 0) // find west neighbour
		{
			neighbours = (ChunkRLE *)(neighborChunks[3]);
			if (!neighbours || (neighbours->posX != this->posX - 1 && neighbours->posY == this->posY))
			{
				return (NULL);
			}
			pos = neighbours->GetRubanPos(sizeX - 1, y, z);
			return (neighbours->rubans);
		}
		pos = GetRubanPos(x - 1, y, z);
		break;
	}
	}

	return (this->rubans);
}

#define t_height 0
#define t_ruban_pos 1
#define t_block_type 2
#define t_face_type 3

void ChunkRLE::CompileTopBotFaces()
{
	std::vector<std::vector<std::vector<u_int>>> tool;

	// std::cout << "chunk : " << this->posX << " " << this->posY << std::endl;
	toolInit(tool);
	u_char map[Chunk::sizeX * Chunk::sizeY];
	for (u_int y = 0; y < Chunk::sizeY; y++)
		for (u_int x = 0; x < Chunk::sizeX; x++)
			map[x + y * Chunk::sizeX] = 1;

	u_char *ruban = this->rubans->data();

	for (u_int z = 0; z < Chunk::sizeZ; /* z++ */)
	{
		u_int lowest = Chunk::sizeZ;
		for (u_int y = 0; y < Chunk::sizeY; y++)
		{
			for (u_int x = 0; x < Chunk::sizeX; x++)
			{
				// find next face to draw in ruban
				while (map[x + y * Chunk::sizeX] && tool[x][y][t_height] < sizeZ - 1)
				{

					if (tool[x][y][t_block_type] == AIR) // si sur AIR
					{
						tool[x][y][t_height] += ruban[tool[x][y][t_ruban_pos] + 1];
						tool[x][y][t_ruban_pos] += 2;

						if (tool[x][y][t_height] >= sizeZ - 1) // si end
						{
							tool[x][y][t_face_type] = 0;
							break;
						}

						tool[x][y][t_block_type] = ruban[tool[x][y][t_ruban_pos]];
						tool[x][y][t_face_type] = 4;
						continue;
					}

					if (tool[x][y][t_face_type] == 5) // si face top a faire
					{
						// trouver si next ruban == AIR
						if (tool[x][y][t_height] + ruban[tool[x][y][t_ruban_pos] + 1] >= sizeZ - 1)
							break;
						if (ruban[tool[x][y][t_ruban_pos] + 2] == AIR)
							break;
						tool[x][y][t_height] += ruban[tool[x][y][t_ruban_pos] + 1];
						tool[x][y][t_ruban_pos] += 2;
						tool[x][y][t_block_type] = ruban[tool[x][y][t_ruban_pos]];
						continue;
					}
					break;
				}
				// if (posX == 0 && posY == 1 && x == 2 && (y == 4 || y == 5))
				// {
				// 	std::cout << "x : " << x << " y : " << y << " z : " << tool[x][y][height] << " face : " << (int)tool[x][y][t_face_type] << " block : " << (int)tool[x][y][block_type] << std::endl;
				// }
				if (tool[x][y][t_face_type])
					map[x + y * Chunk::sizeX] = 0;

				u_int thisLow = Chunk::sizeZ;

				if (tool[x][y][t_face_type] == 4)
					thisLow = tool[x][y][t_height];
				if (tool[x][y][t_face_type] == 5)
					thisLow = tool[x][y][t_height] + ruban[tool[x][y][t_ruban_pos] + 1] - 1;

				if (thisLow < lowest)
				{
					lowest = thisLow;
				}
			}
		}
		// make face
		makeTopBotFaces(tool, ruban, lowest, map);
		z = lowest; // find du lowest a opti !!!
	}
}

void ChunkRLE::toolInit(std::vector<std::vector<std::vector<u_int>>> &tool)
{
	tool.resize(sizeX);
	for (u_int x = 0; x < sizeX; x++)
		tool[x].resize(sizeY);

	for (u_int y = 0; y < sizeY; y++)
	{
		for (u_int x = 0; x < sizeX; x++)
		{
			tool[x][y].resize(4);
			tool[x][y][0] = 0;											   // height
			tool[x][y][1] = GetRubanPos(x, y, 0);						   // ruban pos
			tool[x][y][2] = this->rubans->data()[tool[x][y][t_ruban_pos]]; // block type
			tool[x][y][3] = 4;											   // face type
		}
	}
}

void ChunkRLE::makeTopBotFaces(std::vector<std::vector<std::vector<u_int>>> &tool, u_char *ruban, u_int lowest, u_char *map)
{
	// tool[t_face_type] quand draw 4 -> 5 quand draw 5 -> 0

	for (u_int y = 0; y < sizeY; y++)
	{
		for (u_int x = 0; x < sizeX; x++)
		{

			// algo de face making
			if (map[x + y * Chunk::sizeX] != 0 || !tool[x][y][t_face_type] || tool[x][y][t_face_type] == 4 && tool[x][y][t_height] != lowest || tool[x][y][t_face_type] == 5 && tool[x][y][t_height] + ruban[tool[x][y][t_ruban_pos] + 1] - 1 != lowest)
				continue;

			map[x + y * Chunk::sizeX] = 1;
			u_int longX = 1;
			u_int longY = 1;
			for (u_int px = x + 1; px < sizeX; px++)
			{
				if (map[px + y * Chunk::sizeX] != 0 || tool[px][y][t_height] != lowest || tool[x][y][t_face_type] != tool[px][y][t_face_type] || tool[x][y][t_block_type] != tool[px][y][t_block_type] || tool[px][y][t_height] != tool[x][y][height])
					break;
				longX++;

				map[px + y * Chunk::sizeX] = 1;
				if (tool[px][y][t_face_type] == 4)
				{
					tool[px][y][t_face_type] = 5;

					continue;
				}
				if (tool[px][y][t_face_type] == 5)
				{
					tool[px][y][t_face_type] = 0;
					tool[px][y][t_height] += ruban[tool[px][y][t_ruban_pos] + 1];
					tool[px][y][t_ruban_pos] += 2;
					if (tool[px][y][t_ruban_pos] < rubans->size())
					{
						tool[px][y][t_block_type] = ruban[tool[px][y][t_ruban_pos]];
					}
					continue;
				}
			}

			for (u_int py = y + 1; py < sizeY; py++)
			{
				int tmpX = 0;
				for (u_int px = x; px < longX; px++)
				{
					if (map[px + py * Chunk::sizeX] != 0 || tool[px][py][t_height] != lowest || tool[x][y][t_face_type] != tool[px][py][t_face_type] || tool[x][y][t_block_type] != tool[px][py][t_block_type] || tool[px][py][t_height] != tool[x][y][t_height])
						break;
					tmpX++;
				}
				if (tmpX != longX)
					break;
				longY++;
				for (u_int px = x; px < longX; px++)
				{
					map[px + py * Chunk::sizeX] = 1;
					if (tool[px][py][t_face_type] == 4)
					{
						tool[px][py][t_face_type] = 5;
						continue;
					}
					if (tool[px][py][t_face_type] == 5)
					{
						tool[px][py][t_face_type] = 0;
						tool[px][py][t_height] += ruban[tool[px][py][t_ruban_pos] + 1];
						tool[px][py][t_ruban_pos] += 2;
						if (tool[px][py][t_ruban_pos] < rubans->size())
						{
							tool[px][py][t_block_type] = ruban[tool[px][py][t_ruban_pos]];
						}
						continue;
					}
				}
			}

			if (tool[x][y][t_face_type] == 4)
			{
				CreateFaceRLE(tool[x][y][t_face_type], vertexData, shapeAssemblyData, x, y, tool[x][y][t_height], vertexData.size(), tool[x][y][t_block_type], longX, longY);

				tool[x][y][t_face_type] = 5;

				continue;
			}

			if (tool[x][y][t_face_type] == 5)
			{
				CreateFaceRLE(tool[x][y][t_face_type], vertexData, shapeAssemblyData, x, y, tool[x][y][height] + ruban[tool[x][y][ruban_pos] + 1] - 1, vertexData.size(), tool[x][y][block_type], longX, longY);
				tool[x][y][t_face_type] = 0;
				tool[x][y][height] += ruban[tool[x][y][ruban_pos] + 1];
				tool[x][y][ruban_pos] += 2;
				if (tool[x][y][ruban_pos] < rubans->size())
					tool[x][y][block_type] = ruban[tool[x][y][ruban_pos]];
				continue;
			}
		}
	}
}

void ChunkRLE::CompileSideFaces()
{
	u_int pos = 0;
	for (u_int y = 0; y < sizeY; y++)
	{
		for (u_int x = 0; x < sizeX; x++)
		{
			parkourRubans(x, y, pos);
		}
	}
}

void ChunkRLE::incrementNeighb(int &neighb_pos, int &neighb_z, int &incr, int neighb_size, int &over)
{
	if (over + incr >= neighb_size)
	{
		neighb_pos += 2;
		neighb_z += neighb_size;
		incr -= neighb_size - over;
		over = 0;
		return;
	}
	over += incr;
	incr = 0;
}

void ChunkRLE::parkourRubans(u_int &x, u_int &y, u_int &pos)
{
	u_int z = 0;
	int neighb_pos[4] = {0, 0, 0, 0};  // pos in neighbour's rubans
	int neighb_z[4] = {0, 0, 0, 0};	   // z postion of the neighbour
	int neighb_over[4] = {0, 0, 0, 0}; // nbr of block already checked in this ruban
	while (z < sizeZ - 1)
	{

		u_int z_end = z + data[pos + 1];

		/*Bottom Face*/
		// if ((!z && data[pos]) || (data[pos] && (data[pos - 2] == AIR || data[pos - 2] == WATER)))
		// 	CreateFaceRLE(4, vertexData, shapeAssemblyData, x, y, z, vertexData.size(), data[pos], 1, 1);

		for (u_int neighb = 0; neighb < 4; neighb++)
		{
			std::vector<u_char> *ruban_vec = GetAdjacentRuban(x, y, z, neighb_pos[neighb], neighb); // find neighb of our block's ruban
			u_char *ruban = ruban_vec ? ruban_vec->data() : NULL;
			while (neighb_z[neighb] + neighb_over[neighb] < z_end)
			{
				int real_z = neighb_z[neighb] + neighb_over[neighb];
				int neighb_size = data[pos + 1];
				int to_draw = z_end - z;
				if (!ruban || neighb_pos[neighb] >= ruban_vec->size())
				{
					incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
					continue;
				}

				neighb_size = ruban[neighb_pos[neighb] + 1];

				if (data[pos] == AIR) // if we are at an empty block
				{
					int real_size = ruban[neighb_pos[neighb] + 1] - neighb_over[neighb];
					if (real_z + real_size < z_end)
						to_draw = real_size;
					else
						to_draw = real_size - (real_z + real_size - z_end);
					incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
					continue;
				}

				if (ruban && ((data[pos] == WATER && ruban[neighb_pos[neighb]] == WATER) || (ruban[neighb_pos[neighb]] != WATER && ruban[neighb_pos[neighb]] != AIR))) // if neighb is not an empty block but we dont want to draw it
				{
					int real_size = ruban[neighb_pos[neighb] + 1] - neighb_over[neighb];
					if (real_z + real_size < z_end)
						to_draw = real_size;
					else
						to_draw = real_size - (real_z + real_size - z_end);
					incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
					continue;
				}

				if (ruban)
				{
					int real_size = ruban[neighb_pos[neighb] + 1] - neighb_over[neighb];
					to_draw = 0;
					if (real_z + real_size <= z_end)
						to_draw = real_size;
					else
						to_draw = real_size - (real_z + real_size - z_end);
				}
				// for (int i = 0; i < to_draw; i++)
				// CreateFaceRLE(neighb, vertexData, shapeAssemblyData, x, y, real_z + i, vertexData.size(), data[pos], 1, 1);
				if (to_draw)
					CreateFaceRLE(neighb, vertexData, shapeAssemblyData, x, y, real_z, vertexData.size(), data[pos], 1, to_draw);
				while (to_draw)
				{
					incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
					if (ruban)
						neighb_size = ruban[neighb_pos[neighb] + 1];
				}
			}
		}
		/*Top Face*/
		// if (data[pos] && z < sizeZ - 1 && (data[pos + 2] == AIR || data[pos + 2] == WATER))
		//  	CreateFaceRLE(5, vertexData, shapeAssemblyData, x, y, z_end - 1, vertexData.size(), data[pos], 1, 1);

		pos += 2;
		z = z_end;
	}
}

void ChunkRLE::CompileData()
{
	vertexData.clear();
	shapeAssemblyData.clear();
	vertexData.reserve(500);
	shapeAssemblyData.reserve(500);

	CompileTopBotFaces();
	CompileSideFaces();

	dataStruct.data = (u_char *)vertexData.data();
	dataStruct.size = vertexData.size() * sizeof(int);
}

/*****	1 - constructors 		*****/

ChunkRLE *ChunkRLE::GetNeighbour(int cardinal)
{
	ChunkRLE *neighbour = NULL;

	switch (cardinal)
	{
	case 0:
		neighbour = (ChunkRLE *)loadedChunks[posX % loadedChunks.size()][(posY + 1) % loadedChunks.size()];
		if (neighbour && neighbour->posY == this->posY + 1)
		{
			return (neighbour);
		}
		return (NULL);
	case 1:
		neighbour = (ChunkRLE *)loadedChunks[(posX + 1) % loadedChunks.size()][posY % loadedChunks.size()];
		if (neighbour && neighbour->posX == this->posX + 1)
		{
			return (neighbour);
		}
		return (NULL);
	case 2:
		neighbour = (ChunkRLE *)loadedChunks[posX % loadedChunks.size()][(posY - 1) % loadedChunks.size()];
		if (neighbour && neighbour->posY == this->posY - 1)
		{
			return (neighbour);
		}
		return (NULL);
	case 3:
		neighbour = (ChunkRLE *)loadedChunks[(posX - 1) % loadedChunks.size()][posY % loadedChunks.size()];
		if (neighbour && neighbour->posX == this->posX - 1)
		{
			return (neighbour);
		}
		return (NULL);
	}

	return (NULL);
}

u_int ChunkRLE::GetRubanPos(int x, int y, int z)
{
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;

	while (tmp_z <= z)
	{
		tmp_z += this->data[pos + 1];
		pos += 2;
	}

	return (pos - 2);
}

void ChunkRLE::updateFromRaw(u_char *rawData)
{
	rubans = new std::vector<u_char>;
	rubans->reserve(sizeX * sizeY * 2);

	u_int pos = 0;
	if (!rawData)
	{
		std::cout << "GEN CORRUPTED" << std::endl;
		exit(6);
	}

	for (u_int y = 0; y < sizeY; y++)
	{
		for (u_int x = 0; x < sizeX; x++)
		{

			rubansIndexes[x][y] = rubans->size();

			u_char type = 0;
			u_char size = 0;

			for (u_int z = 0; z < sizeZ; z++)
			{
				if (rawData[pos] != type)
				{
					if (size)
					{
						rubans->push_back(type);
						rubans->push_back(size);
					}
					type = rawData[pos];
					size = 1;
				}
				else
					size++;
				pos++;
			}
			rubans->push_back(0);
			rubans->push_back(sizeZ - 1);
		}
	}

	this->sizeData = rubans->size();
	this->data = rubans->data();

	free(rawData);
}

void ChunkRLE::CreatePointVertexGeometry(std::vector<int> &vertexes, int pos, u_char orientation, u_char type, u_char longX, u_char longY)
{
	vertexes.push_back(posX); // 8 bytes
	vertexes.push_back(posY); // 8 bytes
	int tmp = 0;
	tmp += longY;
	tmp = (int)pos << 16;
	tmp += orientation << 12;
	tmp += type << 8;
	tmp += longX << 4;
	// std::cout << "geo_IN_pos : " << pos << "  " << (int)orientation << "  " << (int)type << "  " << (int)longX << "  " << (int)longY << std::endl;
	int geo_out_pos = 0;
	u_char geo_out_face = 0;
	u_char geo_out_type = 0;
	u_char geo_out_longX = 0;
	u_char geo_out_longY = 0;

	geo_out_longY = tmp & 0xF;
	geo_out_longX = tmp >> 4 & 0xF;
	geo_out_type = tmp >> 8 & 0xF;
	geo_out_face = tmp >> 12 & 0xF;
	geo_out_pos = tmp >> 16 & 0xFFFF;
	// std::cout << "geo_out_pos : " << geo_out_pos << "  " << (int)geo_out_face << "  " << (int)geo_out_type << "  " << (int)geo_out_longX << "  " << (int)geo_out_longY << std::endl << std::endl;
	// vertexes.push_back(tmp);

	vertexes.push_back(pos);		 // 4 bytes
	vertexes.push_back(orientation); // 1 byte (peut etre moins)
	vertexes.push_back(type);		 // 1 byte
	vertexes.push_back(longX);		 // 1 byte
	vertexes.push_back(longY);		 // 1 byte
}

void ChunkRLE::CreateFaceRLEGeometry(int orientation, std::vector<int> &vData, std::vector<u_int> &iData, int x, int y, int z, int offset, u_char type, int longX, int longY)
{
	offset = vData.size();

	iData.push_back(offset);

	int pos = x + y * this->sizeX + z * this->sizeX * this->sizeY;

	CreatePointVertexGeometry(vData, pos, orientation, type, longX, longY);
}

void ChunkRLE::CreatePointVertexRegular(std::vector<int> &vertexes, int pos, u_char orientation, u_char type)
{
	for (char i = 0; i < 4; i++)
	{
		vertexes.push_back(posX);
		vertexes.push_back(posY);
		vertexes.push_back(pos);
		vertexes.push_back(orientation);
		vertexes.push_back(type);

		if (orientation == 2 || orientation == 3 || orientation == 4)
			vertexes.push_back(8);
		else
			vertexes.push_back(10);
	}
}

void ChunkRLE::CreateFaceRLERegular(int orientation, std::vector<int> &vData, std::vector<u_int> &iData, int x, int y, int z, int offset, u_char type)
{

	offset = vData.size() / 6;

	iData.push_back(0 + offset);
	iData.push_back(1 + offset);
	iData.push_back(2 + offset);

	iData.push_back(0 + offset);
	iData.push_back(2 + offset);
	iData.push_back(3 + offset);

	int pos = x + y * this->sizeX + z * this->sizeX * this->sizeY;

	CreatePointVertexRegular(vData, pos, orientation, type);
}

void ChunkRLE::CreateFaceRLE(int orientation, std::vector<int> &vData, std::vector<u_int> &iData, int x, int y, int z, int offset, u_char type, int sizeX, int sizeY)
{
	if (this->shaderName == (char *)"RLE-Geometry")
	{
		CreateFaceRLEGeometry(orientation, vData, iData, x, y, z, offset, type, sizeX, sizeY);
		return;
	}
	else if (this->shaderName == (char *)"RLE-Regular")
	{
		CreateFaceRLERegular(orientation, vData, iData, x, y, z, offset, type);
		return;
	}
}