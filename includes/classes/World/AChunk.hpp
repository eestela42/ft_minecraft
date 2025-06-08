#ifndef ACHUNK_HPP

#define ACHUNK_HPP

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include <memory>

#include <classes/Profiler.hpp>

#include <classes/types.hpp>

# define NEIGHB_NORTH 0
# define NEIGHB_SOUTH 1
# define NEIGHB_EAST 2
# define NEIGHB_WEST 3

struct IVec2Hash {
    std::size_t operator()(const glm::ivec2& v) const {
        // std::size_t h1 = std::hash<int>{}(v.x);
        // std::size_t h2 = std::hash<int>{}(v.y);
        // return h1 ^ (h2 << 1); // Combine hashes
		return (std::size_t)v.x * 1000000 + v.y;
    }
};

struct IVec2Equal {
    bool operator()(const glm::ivec2& lhs, const glm::ivec2& rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

int mod_floor(int a, int n);

class AChunk;

typedef struct t_neighbours
{
	AChunk* north;
	AChunk* south;
	AChunk* east;
	AChunk* west;
} s_neighbours;


class AChunk {

protected :

	u_int sharedHolder = 1;

private:
	
	std::mutex mutex;

	u_char *data = NULL;
	u_char *modifData = NULL;


	

	bool isGenerated = false;
	bool isCompiled = false;
	bool toUpdate = false;

	s_neighbours neighbours;

	virtual void privGenerate(u_char *data) = 0;
	virtual void privCompile() = 0;

	virtual t_vbo_data privGetPtrVertices() = 0;
	virtual std::vector<unsigned int> *privGetPtrIndices() = 0;

	virtual bool privIsFilled(int x, int y, int z) = 0;
	virtual u_char privBlockType(int x, int y, int z) = 0;

	virtual bool privChangeBlock(int x, int y, int z, u_char type) = 0;


public :

	int posX;
	int posY;
	int posZ;


	
	
	static u_int const sizeX = 16;
	static u_int const sizeY = 16;
	static u_int const sizeZ = 256;
	
	AChunk(int x, int y, int z);
	virtual ~AChunk();
	virtual void deleter() = 0;

	
	/*Public Functions*/

	void pubGenerate(u_char *data);
	void pubCompile();

	t_vbo_data pubGetPtrVertices();
	std::vector<unsigned int> *pubGetPtrIndices();
	
	bool pubIsFilled(int x, int y, int z);
	u_char pubBlockType(int x, int y, int z);

	bool pubChangeBlock(int x, int y, int z, u_char type);

	/*Public Setter and Getters*/

	void borrow();

	glm::ivec2 getPos();

	u_char *getData();
	void setData(u_char *data);

	u_char *getDataMutex();
	void setDataMutex(u_char *data);

	void setIsGenerated(bool isGenerated);
	bool getIsGenerated();
	bool getIsGeneratedMutex();


	void setIsCompiled(bool isCompiled);
	bool getIsCompiled();

	void setToUpdate(bool toUpdate);
	bool getToUpdate();

	s_neighbours getNeighbours();
	void setNeighbours(s_neighbours neighbours);
	void setNeighbour(int direction, AChunk* chunk);

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