#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include <classes/World/PerlinNoise.hpp>

#include <classes/vertexData.hpp>
#include <classes/Profiler.hpp>

class Chunk
{
private:

	Chunk();
	static u_int idCount;
	u_int id;
	bool didUpdate = false;
	bool isCompiled = false;
	bool isGenerated = false;

	virtual void CompileData() = 0;
	virtual void updateFromRaw(u_char *rawData) = 0;


protected:
	Chunk(int posX, int posY);
	u_char *data = (u_char *)""; //mettre a NULL ?

	std::vector<u_int> shapeAssemblyData;
	t_vertexData dataStruct;
	int posX;
	int posY;


public:
	static constexpr char *shaderName = (char *)"default";
	static u_int const sizeX = 16;
	static u_int const sizeY = 16;
	static u_int const sizeZ = 256;

	virtual void PublicGenerate() final;
	virtual t_vertexData &GetVertexData() final;
	virtual std::vector<u_int> &GetShapeAssemblyData() final;
	virtual int GetX() final;
	virtual int GetY() final;

	virtual ~Chunk();



	static std::vector<std::vector<Chunk *>> loadedChunks;
	static void setRenderDistance(int renderDistance);

	static const std::vector<std::vector<Chunk *>> &GetLoadedChunks();


};

#define DIRT 1
#define GRASS 2
#define STONE 3
#define SAND 4
#define OAK_WOOD_SIDE 5
#define LEAVES_2 6
#define BEDROCK 7
#define WATER 8
#define SNOW_BLOCK 9
#define GOLD_MINERAL 10
#define IRON_MINERAL 11
#define DIAMOUND_MINERAL 12

#define AIR 0
#define ANDESITH 16
// # define STONE 17
// # define DIRT 18
// # define GRASS 19
#define OAK_PLANCH 20
#define SMOOTH_CUT_SEMI_STONE 21
#define SMOOTH_CUT_STONE 22
#define BRICK 23
#define TNT_SIDE 24
#define TNT_TOP 25
#define TNT_BOT 26
#define SPIDER_WEB 27
#define FLOWER_ROSE 28
#define FLOWER_YELLOW 29
// # define WATER 30
#define OAK_SAPLING 31
#define COBBLE_STONE 32
// # define BEDROCK 33
// # define SAND 34
#define GRAVEL 35
// # define OAK_WOOD_SIDE 36
#define OAK_WOOD_TOP 37
#define IRON_BLOCK 38
#define GOLD_BLOCK 39
#define DIAMOUND_BLOCK 40
#define EMRALD_BLOCK 41
#define PINK_1 42
#define PINK_2 43
#define MUSHROOM_RED 44
#define MUSHROOM_BROWM 45
#define SEKOIA_SAPPLING 46
#define WEIRD_RED 47
// # define GOLD_MINERAL 48
// # define IRON_MINERAL 49
#define CHARCOAL_MINERAL 50
#define BOOKSHELF 51
#define MOSSY_COBBLESTONE 52
#define OBSIDIAN 53
#define IDK 54
#define WILD_GRASS 55
#define WEIRD 56
#define BEACON 57
#define RED_3 58
#define CRAFTING_TABLE_TOP 59
#define FURNACE_FRONT_OFF 60
#define FURNACE_SIDE 61
#define DISPENSER_FRONT 62
#define WEIRD_RED_2 63
#define SPONGE 64
// # define DIAMOUND_MINERAL 66
#define LEAVES_1 68
// # define LEAVES_2 69
// # define SNOW_BLOCK 82
#define PUMKIN_FACE 135

#endif