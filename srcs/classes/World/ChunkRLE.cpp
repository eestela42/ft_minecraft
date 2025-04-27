#include <classes/World/ChunkRLE.hpp>
#include <map>
#include <classes/World/PerlinNoise.hpp>
#include <iostream>
#include <cstring>

ChunkRLE::ChunkRLE(int posX, int posY, int posZ) : AChunk(posX, posY, posZ)
{
	this->rubans = NULL;
}

ChunkRLE::~ChunkRLE()
{
	delete this->rubans;
}

void ChunkRLE::deleter()
{
	if (this->sharedHolder == 1)
		delete this;
	else
		this->sharedHolder--;
}


		/*****	2 - methods 		*****/
void ChunkRLE::CreatePointVertexGeometry(std::vector<int> &vertexes, int pos, u_char orientation, u_char type, u_char longX, u_char longY)
{
		vertexes.push_back(posX); //4 bytes
		vertexes.push_back(posY);//4 bytes
		vertexes.push_back(pos);//4 bytes
		int tmp = 0;
		tmp += longY;
		tmp += longX << 8;
		tmp += type << 16;
		tmp += orientation << 24;

		u_char geo_out_face = 0;
		u_char geo_out_type = 0;
		u_char geo_out_longX = 0;
		u_char geo_out_longY = 0;
		
		geo_out_longY = tmp & 0xFF;
		geo_out_longX = tmp >> 8 & 0xFF;
		geo_out_type = tmp >> 16 & 0xFF;
		geo_out_face = tmp >> 24 & 0xFF;

		vertexes.push_back(tmp);
		
		if (longY != longY || longX != longX || geo_out_face != orientation || geo_out_type != type || geo_out_longX != longX || geo_out_longY != longY)
		{
			std::cout << (int)orientation << "  " << (int)type << "  " << (int)longX << "  " << (int)longY << std::endl;
			std::cout << (int)geo_out_face << "  " << (int)geo_out_type << "  " << (int)geo_out_longX << "  " << (int)geo_out_longY << std::endl << std::endl;
		}

		// vertexes.push_back(pos);//4 bytes
		// vertexes.push_back(orientation);//1 byte (peut etre moins)
		// vertexes.push_back(type);//1 byte
		// vertexes.push_back(longX);//1 byte
		// vertexes.push_back(longY);//1 byte
}

void ChunkRLE::CreateFaceRLEGeometry(int orientation, std::vector<int> &vData, std::vector<u_int> &iData, int x, int y, int z, int offset, u_char type, int longX, int longY) {
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

void ChunkRLE::CreateFaceRLERegular(int orientation, std::vector<int> &vData, std::vector<u_int> &iData, int x, int y, int z, int offset, u_char type) {
	
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
	if (this->shaderName == (char*)"RLE-Geometry")
	{
		CreateFaceRLEGeometry(orientation, vData, iData, x, y, z, offset, type, sizeX, sizeY);
		return ;
	}
	else if (this->shaderName == (char*)"RLE-Regular")
	{
		CreateFaceRLERegular(orientation, vData, iData, x, y, z, offset, type);
		return ;
	}
}





std::vector<u_char>* 	ChunkRLE::GetAdjacentRuban(int x, int y, int z, int &pos, u_char direction)
{

	ChunkRLE* 				neighbours;


	switch(direction) //pos du neighbour
	{
		case 0: //north
		{
			if (y == sizeY - 1){	//find north neighbour
				neighbours = (ChunkRLE*)(getNeighbours().north);
				if (!neighbours || (neighbours->posY != this->posY + 1  && neighbours->posX == this->posX))
				{
					std::cout << "no north neighbour" << std::endl;
					return (NULL);
				}
				pos = neighbours->GetRubanPos(x, 0, z);
				return (neighbours->rubans);
			}
			pos = GetRubanPos(x, y + 1, z);
			break;
		}
		case 1: //east
		{
			if (x == sizeX - 1) //find east neighbour
			{	
				neighbours = (ChunkRLE*)(getNeighbours().east);
				if (!neighbours || (neighbours->posX != this->posX + 1  && neighbours->posY == this->posY))
				{
					std::cout << "no east neighbour" << std::endl;
					return (NULL);
				}
				pos = neighbours->GetRubanPos(0, y, z);
				return (neighbours->rubans);
			}
			pos = GetRubanPos(x+1, y, z);
			break;
		}
		case 2: //south
		{
			if (y == 0)
			{	//find south neighbour
				neighbours = (ChunkRLE*)(getNeighbours().south);
				if (!neighbours || (neighbours->posY != this->posY - 1  && neighbours->posX == this->posX))
				{
					std::cout << "no south neighbour" << std::endl;
					return (NULL);
				}
				pos = neighbours->GetRubanPos(x, sizeY - 1, z);
				return (neighbours->rubans);
			}
			pos = GetRubanPos(x, y-1, z);
			break;
		}
		case 3: //west
		{
			if (x == 0)//find west neighbour
			{
				neighbours = (ChunkRLE*)(getNeighbours().west);
				if (!neighbours || (neighbours->posX != this->posX - 1 && neighbours->posY == this->posY))
				{
					std::cout << "no west neighbour" << std::endl;
					return (NULL);
				}
				pos = neighbours->GetRubanPos(sizeX - 1, y, z);
				return (neighbours->rubans);
			}
			pos = GetRubanPos(x-1, y, z);
			break;
		}
	}

	return (this->rubans);
}

void	ChunkRLE::incrementNeighb(int& neighb_pos, int& neighb_z, int& incr, int neighb_size, int& over)
{
	if (over + incr >= neighb_size)
	{
		neighb_pos += 2;
		neighb_z += neighb_size;
		incr -= neighb_size - over;
		over = 0;
		return ;
	}
	over += incr;
	incr = 0;
}

#define height 0
#define ruban_pos 1
#define block_type 2
#define face_type 3

void 		ChunkRLE::makeTopBotFaces(std::vector<std::vector<std::vector<u_int>>> &tool, u_char *ruban, u_int lowest, u_char *map)
{
	//tool[face_type] quand draw 4 -> 5 quand draw 5 -> 0


	
	for (u_int y = 0; y < sizeY; y++)
	{
	for (u_int x = 0; x < sizeX; x++)
	{
		

		// algo de face making
		if (map[x + y * AChunk::sizeX] != 0 || !tool[x][y][face_type]
			|| tool[x][y][face_type] == 4 && tool[x][y][height] != lowest
			|| tool[x][y][face_type] == 5 && tool[x][y][height] + ruban[tool[x][y][ruban_pos] + 1] - 1 != lowest)
			continue ;
		
		map[x + y * AChunk::sizeX] = 1;
		u_int longX = 1;
		u_int longY = 1;
		for (u_int px = x + 1; px < sizeX; px++)
		{
			if (map[px + y * AChunk::sizeX] != 0 || tool[px][y][height] != lowest || tool[x][y][face_type] !=  tool[px][y][face_type]
				||  tool[x][y][block_type]  !=  tool[px][y][block_type] || tool[px][y][height] != tool[x][y][height])
				break ;
			longX++;

			map[px + y * AChunk::sizeX] = 1;
			if (tool[px][y][face_type] == 4)
			{
				tool[px][y][face_type] = 5;
				
				continue ;
			}
			if (tool[px][y][face_type] == 5)
			{
				tool[px][y][face_type] = 0;
				tool[px][y][height] += ruban[tool[px][y][ruban_pos] + 1];
				tool[px][y][ruban_pos] += 2;
				if (tool[px][y][ruban_pos] < rubans->size())
				{
					tool[px][y][block_type] = ruban[tool[px][y][ruban_pos]];
				}
				continue ;
			}
		}

		for (u_int py = y + 1; py < sizeY; py++)
		{
			int tmpX = 0;
			for (u_int px = x; px < longX; px++)
			{
				if (map[px + py * AChunk::sizeX] != 0 || tool[px][py][height] != lowest || tool[x][y][face_type] !=  tool[px][py][face_type]
					||  tool[x][y][block_type]  !=  tool[px][py][block_type]  || tool[px][py][height] != tool[x][y][height])
					break ;
				tmpX++;
			}
			if (tmpX != longX)
				break;
			longY++;
			for (u_int px = x; px < longX; px++)
			{
				map[px + py * AChunk::sizeX] = 1;
				if (tool[px][py][face_type] == 4)
				{
					tool[px][py][face_type] = 5;
					continue ;
				}
				if (tool[px][py][face_type] == 5)
				{
					tool[px][py][face_type] = 0;
					tool[px][py][height] += ruban[tool[px][py][ruban_pos] + 1];
					tool[px][py][ruban_pos] += 2;
					if (tool[px][py][ruban_pos] < rubans->size())
					{
						tool[px][py][block_type] = ruban[tool[px][py][ruban_pos]];
					}
					continue ;
				}
			}
		}


		if (tool[x][y][face_type] == 4)
		{
			CreateFaceRLE(tool[x][y][face_type], vertexData, indices, x, y, tool[x][y][height], vertexData.size(), tool[x][y][block_type], longX, longY);
			
			tool[x][y][face_type] = 5;
			
			continue ;
		}

		if (tool[x][y][face_type] == 5)
		{
			CreateFaceRLE(tool[x][y][face_type], vertexData, indices, x, y, tool[x][y][height] + ruban[tool[x][y][ruban_pos] + 1] - 1, vertexData.size(), tool[x][y][block_type], longX, longY);
			tool[x][y][face_type] = 0;
			tool[x][y][height] += ruban[tool[x][y][ruban_pos] + 1];
			tool[x][y][ruban_pos] += 2;
			if (tool[x][y][ruban_pos] < rubans->size())
				tool[x][y][block_type] = ruban[tool[x][y][ruban_pos]];
			continue ;
		}
	}
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
			tool[x][y][0] = 0;										//height
			tool[x][y][1] = GetRubanPos(x, y, 0);					//ruban pos
			tool[x][y][2] = this->rubans->data()[tool[x][y][ruban_pos]];	//block type
			tool[x][y][3] = 4;										//face type
		}
	}
}


void	ChunkRLE::CompileTopBotFaces()
{
	std::vector<std::vector<std::vector<u_int>>> tool;

	// std::cout << "chunk : " << this->posX << " " << this->posY << std::endl;
	toolInit(tool);
	u_char map[AChunk::sizeX * AChunk::sizeY];
	for (u_int y = 0; y < AChunk::sizeY; y++)
		for (u_int x = 0; x < AChunk::sizeX; x++)
			map[x + y * AChunk::sizeX] = 1;
	
	u_char *ruban = this->rubans->data();

	for (u_int z = 0; z < AChunk::sizeZ;/* z++ */)
	{
	u_int lowest = AChunk::sizeZ;
	for (u_int y = 0; y < AChunk::sizeY; y++)
	{
	for (u_int x = 0; x < AChunk::sizeX; x++)
	{
		//find next face to draw in ruban
		while (map[x + y * AChunk::sizeX] && tool[x][y][height] < sizeZ - 1 )
		{
			
			if (tool[x][y][block_type] == AIR) //si sur AIR
			{
				tool[x][y][height] += ruban[tool[x][y][ruban_pos] + 1];
				tool[x][y][ruban_pos] += 2;

				if (tool[x][y][height] >= sizeZ - 1) // si end
				{
					tool[x][y][face_type] = 0;
					break;
				}

				tool[x][y][block_type] = ruban[tool[x][y][ruban_pos]];
				tool[x][y][face_type] = 4;
				continue ;
			}

			if (tool[x][y][face_type] == 5) //si face top a faire
			{
				//trouver si next ruban == AIR
				if (tool[x][y][height] + ruban[tool[x][y][ruban_pos] + 1] >= sizeZ - 1)
					break;
				if (ruban[tool[x][y][ruban_pos] + 2] == AIR)
					break;
				tool[x][y][height] += ruban[tool[x][y][ruban_pos] + 1];
				tool[x][y][ruban_pos] += 2;
				tool[x][y][block_type] = ruban[tool[x][y][ruban_pos]];
				continue ;
			}
			break ;
		}
		// if (posX == 0 && posY == 1 && x == 2 && (y == 4 || y == 5))
		// {
		// 	std::cout << "x : " << x << " y : " << y << " z : " << tool[x][y][height] << " face : " << (int)tool[x][y][face_type] << " block : " << (int)tool[x][y][block_type] << std::endl;
		// }
		if (tool[x][y][face_type])
			map[x + y * AChunk::sizeX] = 0;
		
		u_int thisLow = AChunk::sizeZ;
		
		if (tool[x][y][face_type] == 4)
			thisLow = tool[x][y][height];
		if (tool[x][y][face_type] == 5)
			thisLow = tool[x][y][height] + ruban[tool[x][y][ruban_pos] + 1] - 1;
		
		
		if (thisLow < lowest)
		{
			lowest = thisLow;
		}
		
	}	
	}
	//make face
	makeTopBotFaces(tool, ruban, lowest, map);
	z = lowest; // find du lowest a opti !!!
	}
}

void	ChunkRLE::parkourRubans(u_int &x, u_int &y, u_int &pos)
{
		u_int z = 0;
		int neighb_pos[4] = {0, 0, 0, 0}; 	//pos in neighbour's rubans
		int neighb_z[4] = {0, 0, 0, 0};		//z postion of the neighbour
		int neighb_over[4] = {0, 0, 0, 0};	//nbr of block already checked in this ruban
		const u_char *data = rubans->data();
		while (z < sizeZ - 1)
		{

			u_int z_end  = z + data[pos + 1];
			
			/*Bottom Face*/
			// if ((!z && data[pos]) || (data[pos] && (data[pos - 2] == AIR || data[pos - 2] == WATER)))
			// 	CreateFaceRLE(4, vertexData, shapeAssemblyData, x, y, z, vertexData.size(), data[pos], 1, 1);
			
			for (u_int neighb = 0; neighb < 4; neighb++)
			{
				std::vector<u_char> *ruban_vec = GetAdjacentRuban(x, y , z, neighb_pos[neighb], neighb); //find neighb of our block's ruban
				u_char *ruban = ruban_vec ? ruban_vec->data() : NULL;
				while (neighb_z[neighb] + neighb_over[neighb] < z_end)
				{
					int real_z = neighb_z[neighb] + neighb_over[neighb];
					int neighb_size = data[pos + 1];
					int to_draw = z_end - z;
					if (!ruban || neighb_pos[neighb] >= ruban_vec->size())
					{
						incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
						continue ;
					}
					
					neighb_size = ruban[neighb_pos[neighb] + 1];

					if (data[pos] == AIR)  //if we are at an empty block
					{
							int real_size = ruban[neighb_pos[neighb] + 1] - neighb_over[neighb];
							if (real_z + real_size < z_end)
								to_draw = real_size;
							else
								to_draw = real_size - (real_z + real_size - z_end);
							incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
							continue ;
					}

					if (ruban && ((data[pos] == WATER && ruban[neighb_pos[neighb]] == WATER)
									|| (ruban[neighb_pos[neighb]] != WATER && ruban[neighb_pos[neighb]] != AIR))) //if neighb is not an empty block but we dont want to draw it
					{
						int real_size = ruban[neighb_pos[neighb] + 1] - neighb_over[neighb];
						if (real_z + real_size < z_end)
							to_draw = real_size;
						else
							to_draw = real_size - (real_z + real_size - z_end);
						incrementNeighb(neighb_pos[neighb], neighb_z[neighb], to_draw, neighb_size, neighb_over[neighb]);
						continue ;
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
						CreateFaceRLE(neighb, vertexData, indices, x, y, real_z, vertexData.size(), data[pos], 1, to_draw);
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

void ChunkRLE::CompileSideFaces()
{
	u_int pos = 0;
	for (u_int y = 0; y < sizeY; y++) {
	for (u_int x = 0; x < sizeX; x++)
	{
		parkourRubans(x, y, pos);
	}
	}
}

void	ChunkRLE::privCompile()
{

	vertexData.clear();
	indices.clear();
	vertexData.reserve(500);
	indices.reserve(500);
	
	CompileTopBotFaces();
	CompileSideFaces();


}

int ChunkRLE::calcX(int pos)
{
	return ((pos/2) % this->sizeX);
}

int ChunkRLE::calcY(int pos)
{
	return ((pos/2) / this->sizeX);
}

u_int					ChunkRLE::GetRubanPos(int x, int y, int z)
{
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;
	const u_char *data = rubans->data();

	while (tmp_z <= z )
	{
		tmp_z += data[pos + 1];
		pos += 2;
	}
	
	return (pos - 2);
}


void ChunkRLE::privGenerate(u_char *rawData)
{
	rubans = new std::vector<u_char>;
	rubans->reserve(sizeX * sizeY * 2);
	
	u_int pos = 0;
	if (!rawData)
	{
		std::cout << "GEN CORRUPTED" << std::endl;
		exit(6);
	}

	for (u_int y = 0; y < sizeY; y++) {
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
	this->setData(rubans->data());

	free(rawData);
}


t_vbo_data 					ChunkRLE::privGetPtrVertices()
{
	t_vbo_data data;
	data.data = vertexData.data();
	data.size = vertexData.size() * sizeof(int);
	return data;
}

std::vector<unsigned int>*	ChunkRLE::privGetPtrIndices()
{
	return &indices;
}

bool ChunkRLE::privIsFilled(int x, int y, int z)
{
	if (!getIsGenerated()|| z < 0 || z >= sizeZ) {
		return false;
	}

	const u_char *data = rubans->data();
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;

	while (tmp_z <= z)
	{
		tmp_z += data[pos + 1];
		pos += 2;
	}
	if (data[pos - 2] != 0)
	{
		return true;
	}
	return false;
}

u_char ChunkRLE::privBlockType(int x, int y, int z) {
	if (!getIsGeneratedMutex()) { 
		return 0;
	}
	const u_char *data = rubans->data();
	int pos = this->rubansIndexes[x][y];
	int tmp_z = 0;
	while (tmp_z <= z)
	{
		tmp_z += data[pos + 1];
		pos += 2;
	}
	return data[pos - 2];
	return 0;
}

bool ChunkRLE::privChangeBlock(int x, int y, int z, u_char type) {
	
	if (x < 0 || x >= sizeX || y < 0 || y >= sizeY || z < 0 || z >= sizeZ) {
		return false;
	}
	std::cout << "RLE CHANGE x : " << x << " y : " << y << " z : " << z << std::endl;
	u_char *data = rubans->data();
	int pos_next = this->rubansIndexes[x][y];
	int z_next = 0;
	while (z_next <= z && z_next < sizeZ)
	{
		z_next += data[pos_next + 1];
		pos_next += 2;
	}
	int pos_before = pos_next - 2;
	//block is the same type as what it is supposed to replace
	if (data[pos_before] == type) {
		return false;
	}
	//block is at the start of a ruban
	int z_before  = z_next - data[pos_before + 1];

	if (data[pos_before + 1] == 1)
	{
		data[pos_before] = type;
		//add modif memory
		return true;
	}

	data[pos_before] = type;

	// if (z == z_before)
	// {
	// 	rubans->insert(rubans->begin() + pos_next, 1 * 2, 0);
	// 	u_char old_type = data[pos_before];
	// 	data[pos_before] = type;
	// 	data[pos_before + 1] = 1;
	// 	data[pos_next] = old_type;
	// 	data[pos_next + 1]--;
	// }
	// else if (z == z_before + data[pos_before + 1] - 1)
	// {
	// 	if (data[pos_before + 2] == type)
	// 	{
	// 		data[pos_before + 1]--;
	// 		data[pos_before + 2]++;
	// 		return true;
	// 	}
	// 	rubans->insert(rubans->begin() + pos_next, 1 * 2, 0);
	// 	data[pos_before + 1]--;
	// 	data[pos_next] = type;
	// 	data[pos_next + 1] = 1;
	// }
	// else
	// {
	// 	rubans->insert(rubans->begin() + pos_next, 2 * 2, 0);
	// 	u_char old_type = data[pos_before];
	// 	data[pos_before + 1]--;
	// 	data[pos_next] = type;
	// 	data[pos_next + 1] = 1;
	// }

	// while (x < sizeX)
	// {
	// 	while (y < sizeY)
	// 	{
	// 		this->rubansIndexes[x][y] += 2;
	// 		y++;
	// 	}
	// 	y = 0;
	// 	x++;
	// }

	//add modif memory
	return true;
}