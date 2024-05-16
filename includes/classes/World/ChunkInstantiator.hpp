#pragma once

#include <classes/World/ChunkDefault.hpp>
#include <classes/World/ChunkRLE.hpp>
#include <classes/World/ChunkGenerator.hpp>
#include <classes/VAO/VertexArrayObjectHandler.hpp>
#include <classes/ShaderHandler.hpp>
#include <unordered_map>
#include <chrono>

struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2> &p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2;
	}
};

class ChunkInstantiator
{
private:
	std::unordered_map<Chunk *, u_int> chunkMap;
	std::unordered_map<std::pair<int, int>, Chunk *, pair_hash> generationQueueMap;
	std::unordered_map<std::pair<int, int>, Chunk *, pair_hash> compilationQueueMap;
	std::unordered_map<std::pair<int, int>, Chunk *, pair_hash> updateQueueMap;
	VertexArrayObjectHandler *vertexArrayObjectHandler;
	int playerChunkPosX;
	int playerChunkPosY;
	int renderDistance;
	ShaderHandler *shaderHandler;

public:
	~ChunkInstantiator();
	ChunkInstantiator(VertexArrayObjectHandler *vertexArrayObjectHandler, int renderDistance, ShaderHandler *shaderHandler);
	bool putBlock(glm::vec3 pos, u_char type);
	// void updateGen(const char *filePath);
	void Update(glm::vec3 playerPos, std::chrono::milliseconds timeBudget);
};