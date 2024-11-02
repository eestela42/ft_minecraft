#ifndef CHUNKINSTANCIATOR_HPP
# define CHUNKINSTANCIATOR_HPP

#include <classes/World/ChunkClassic.hpp>
#include <classes/World/ChunkRLE.hpp>
#include <glm/glm.hpp>
#include <deque>

#include <classes/World/ChunkGenerator.hpp>



class ChunkInstanciator
{
	private :

	ChunkGenerator generator;


	std::vector<std::vector<AChunk*>> tabChunks;
	glm::ivec2 position;

	int renderDistance;
	int generationDistance;
	int size_tab;

	glm::vec3 &realPlayerPos;
	std::mutex &realPlayerPos_mutex;

	void deleteBadChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);
	void createGoodChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);
	void updateChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);
	bool compileChunksWithNeighbours(AChunk *chunk, s_neighbours neighbours);
	

	s_neighbours getNeighbours(glm::ivec2 tabPos, glm::ivec2 chunkPos);

	void setChunkNeighbours(AChunk *chunk, s_neighbours neighbours);

	std::vector<AChunk*> toCompile;


	std::mutex &to_VAO_mutex;
	std::deque<info_VAO*> &to_VAO;

	std::mutex &toDeleteVAO_mutex;
	std::deque<glm::ivec2> &toDeleteVAO;

	public :

	ChunkInstanciator(u_int renderDistance,
						glm::vec3 &playerPos, std::mutex &playerPos_mutex,
						std::deque<info_VAO*> &to_VAO, std::mutex &to_VAO_mutex,
						std::deque<glm::ivec2> &toDeleteVAO, std::mutex &toDeleteVAO_mutex);
	~ChunkInstanciator();

	void update();

	glm::vec2 convertToTabPos(glm::vec2 pos);

};



#endif