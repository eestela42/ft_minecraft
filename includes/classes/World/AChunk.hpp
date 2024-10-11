#ifndef ACHUNK_HPP

#define ACHUNK_HPP

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <mutex>

#include <classes/types.hpp>


class AChunk {

private:
	
	u_char *data = NULL;
	int posX;
	int posY;
	int posZ;

	bool isGenerated = false;
	bool isCompiled = false;
	bool toUpdate = false;

	


public :

	std::vector<Vertex> 		vertices;
	std::vector<unsigned int> 	indices;
	
	std::vector<Vertex> *getPtrVertices();
	std::vector<unsigned int> *getPtrIndices();

	static u_int const sizeX = 16;
	static u_int const sizeY = 16;
	static u_int const sizeZ = 256;


	AChunk(int x, int y, int z);
	~AChunk();

	virtual void publicGenerate() = 0;
	virtual void updateFromRaw(u_char *data) = 0;
	virtual void publicCompile() = 0;

	glm::ivec2 getPos();

	void setData(u_char *data);
	u_char *getData();

	virtual void draw() = 0;

	void setIsGenerated(bool isGenerated);
	bool getIsGenerated();

	void setIsCompiled(bool isCompiled);
	bool getIsCompiled();

	void setToUpdate(bool toUpdate);
	bool getToUpdate();



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