#ifndef ACHUNK_HPP

#define ACHUNK_HPP

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>

#include <classes/types.hpp>

# define NEIGHB_NORTH 0
# define NEIGHB_SOUTH 1
# define NEIGHB_EAST 2
# define NEIGHB_WEST 3
# define NEIGHB_NORTH_EAST 4
# define NEIGHB_NORTH_WEST 5
# define NEIGHB_SOUTH_EAST 6
# define NEIGHB_SOUTH_WEST 7

class AChunk;

typedef struct t_neighbours
{
	AChunk *north;
	AChunk *south;
	AChunk *east;
	AChunk *west;
	AChunk *north_east;
	AChunk *north_west;
	AChunk *south_east;
	AChunk *south_west;
} s_neighbours;


class AChunk {

private:
	
	u_char *data = NULL;
	

	bool isGenerated = false;
	bool isCompiled = false;
	bool toUpdate = false;

	s_neighbours neighbours;


public :

	int posX;
	int posY;
	int posZ;


	
	virtual t_vbo_data getPtrVertices() = 0;
	virtual std::vector<unsigned int> *getPtrIndices() = 0;

	static u_int const sizeX = 16;
	static u_int const sizeY = 16;
	static u_int const sizeZ = 256;


	AChunk(int x, int y, int z);
	~AChunk();

	virtual void updateFromRaw(u_char *data) = 0;
	virtual void publicCompile() = 0;

	glm::ivec2 getPos();

	void setData(u_char *data);
	u_char *getData();



	void setIsGenerated(bool isGenerated);
	bool getIsGenerated();

	void setIsCompiled(bool isCompiled);
	bool getIsCompiled();

	void setToUpdate(bool toUpdate);
	bool getToUpdate();

	virtual bool isFilled(int x, int y, int z);
	virtual u_char blockType(int x, int y, int z);

	bool getIsGenerated() const;
	bool getIsCompiled() const;
	bool getIsToUpdate() const;

	s_neighbours getNeighbours() const;
	void setNeighbours(s_neighbours neighbours);
	void setNeighbour(int direction, AChunk *chunk);

};



# define AIR 0
# define DIRT 1
# define GRASS 2
# define STONE 3
# define SAND 4
# define OAK_WOOD_SIDE 5
# define LEAVES_2 6
# define BEDROCK 7
# define WATER 8
# define SNOW_BLOCK 9
# define GOLD_MINERAL 10
# define IRON_MINERAL 11
# define DIAMOUND_MINERAL 12

#endif