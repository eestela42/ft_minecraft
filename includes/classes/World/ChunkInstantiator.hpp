#pragma once

#include <classes/World/ChunkDefault.hpp>
#include <classes/World/ChunkRLE.hpp>
#include <classes/World/ChunkGenerator.hpp>
#include <classes/VAO/VertexArrayObjectHandler.hpp>
#include <classes/ShaderHandler.hpp>
#include <unordered_map>
#include <chrono>
#include <mutex>

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
	std::vector<Chunk*> toDelete;

	VertexArrayObjectHandler *vertexArrayObjectHandler;
	int playerChunkPosX;
	int playerChunkPosY;
	int renderDistance;
	ShaderHandler *shaderHandler;

public:
	
	~ChunkInstantiator();
	ChunkInstantiator(VertexArrayObjectHandler *vertexArrayObjectHandler, int renderDistance, ShaderHandler *shaderHandler);
	
	void Update(glm::vec3 *playerPos, std::vector<Chunk *> **adrrStableState,
				std::mutex &cameraMutex, std::mutex &stableMutex);

};