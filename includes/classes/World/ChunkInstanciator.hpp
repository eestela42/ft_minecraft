#ifndef CHUNKINSTANCIATOR_HPP
# define CHUNKINSTANCIATOR_HPP

#include <classes/World/ChunkClassic.hpp>
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

};



#endif