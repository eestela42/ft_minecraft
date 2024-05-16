#ifndef RLE_HPP
# define RLE_HPP

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
# include <math.h>
# include <chrono>

# include <classes/World/Chunk.hpp>

	
class ChunkRLE : public Chunk
{
private:
	std::vector<int> vertexData;
	std::vector<u_char> *rubans;

protected :

	std::vector<u_char>* 	GetAdjacentRuban(int x, int y, int z, int &pos, u_char direction);

	u_int* 					rubans_id;
	
	u_int 					rubansIndexes[Chunk::sizeX][Chunk::sizeY];

	u_int 					sizeData = 0;
  
public :
	static constexpr char * shaderName = (char*)"RLE-Geometry";


	~ChunkRLE();
	// ChunkRLE();
	ChunkRLE(int posX, int posY);

	void 				CreatePointVertexGeometry(std::vector<int> &vertexes, int pos, u_char orientation, u_char type, u_char sizeX, u_char sizeY);
	void 				CreatePointVertexRegular(std::vector<int> &vertexes, int pos, u_char orientation, u_char type);

	void				CreateFaceRLE(int orientation, std::vector<int> &vData, std::vector<u_int> &iData,
											int x, int y, int z, int offset, u_char type, int longX, int longY);

	void 				CreateFaceRLEGeometry(int oreientation, std::vector<int> &vData, std::vector<u_int> &iData,
											int x, int y, int z, int offset, u_char type, int longX, int longY);

	void 				CreateFaceRLERegular(int oreientation, std::vector<int> &vData, std::vector<u_int> &iData,
												int x, int y, int z, int offset, u_char type);										

	bool				isFilled(int x, int y, int z) override;
	u_char 				blockType(int x, int y, int z) override;

	u_int				GetRubanPos(int x, int y, int z);
	void				LoadChunk();

	ChunkRLE*			GetNeighbour(int cardinal);
	void				incrementNeighb(int& neighb_pos, int& neighb_z, int& incr, int neighb_size, int& over);




	void				BindNeighbour(int direction, ChunkRLE* chunk);
	
	int 				calcX(int pos);
	int 				calcY(int pos);

	void				toolInit(std::vector<std::vector<std::vector<u_int>>> &tool);

	void 				makeTopBotFaces(std::vector<std::vector<std::vector<u_int>>> &tool, u_char *ruban, u_int lowest, u_char *map);
	void				CompileTopBotFaces();

	void 				CompileSideFaces();


	void				parkourRubans(u_int &x, u_int &y, u_int &pos);

	void 				CompileData() override;

	void 				randomGen(int &pos, int x, int y);
	void 				Generate() override;
	

	void 				updateFromRaw(u_char *rawData) override;

};



#endif