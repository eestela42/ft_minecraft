#ifndef CHUNKGENERATOR_HPP
# define CHUNKGENERATOR_HPP

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <cmath>
#include <glm/glm.hpp>
# include <math.h>
# include <chrono>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

// #include <classes/World/PerlinNoise.hpp>

#include <PerlinNoise.hpp>

#include <classes/World/ChunkClassic.hpp>

#include <classes/Generation/SystemeGen.hpp>


class position
{
public :
	int x;
	int y;

	position(int x, int y) : x(x), y(y) {}

	bool operator==(const position &p) const
	{
		return (x == p.x && y == p.y);
	}

	bool operator<(const position &p) const
	{
		return (x < p.x || (x == p.x && y < p.y));
	}

	bool operator>(const position &p) const
	{
		return (x > p.x || (x == p.x && y > p.y));
	}

	bool operator<=(const position &p) const
	{
		return (x <= p.x || (x == p.x && y <= p.y));
	}

	bool operator>=(const position &p) const
	{
		return (x >= p.x || (x == p.x && y >= p.y));
	}
};

struct param
{
	float geologie;
	float erosion;
	float pique;

	float humidity;
	float temperature;
};

class ChunkGenerator
{
public :
	
	static std::vector<siv::PerlinNoise*> 		noiseList;

	u_char *data;
	
	int posX;
	int posY;

	u_int sizeX = AChunk::sizeX;
	u_int sizeY = AChunk::sizeY;
	u_int sizeZ = AChunk::sizeZ;

	int ground_height;
	int hill_height;

	double p_x;
	double p_y;

	static int seed;

	static std::map<position, std::vector<u_char>*> modifMap;

	std::default_random_engine engine;


	~ChunkGenerator();
	ChunkGenerator();
	ChunkGenerator(u_int seed);
	
	static void initNoise(u_int seed);

	u_char*		generator(glm::ivec2 tmp_pos);
	void		generateTree(int x, int y, int z);
	int 		genBedrock(u_char *data, int x, int y);
	int			genUnderLayer(int pos, int &z);
	int			genOverLayer( int pos, int &z);
	int 		gen2DCave(int hill_height, int pos, int &z);
	int 		genWater(int pos, int level);
	int 		gen3DCave(int hill_height, int pos, int &z);

	int			newGenUnderLayer(int pos, int &z, param p);

	float min_noise = 1000;
	float max_noise = -1000;

	int min_z = 1000;
	int max_z = -1000;
};

#endif