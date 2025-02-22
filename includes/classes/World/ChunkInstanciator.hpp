#ifndef CHUNKINSTANCIATOR_HPP
# define CHUNKINSTANCIATOR_HPP

#include <classes/World/ChunkClassic.hpp>
#include <classes/World/ChunkRLE.hpp>
#include <glm/glm.hpp>
#include <deque>
#include <memory>

#include <classes/World/ChunkGenerator.hpp>



class ChunkInstanciator
{
	private :

	ChunkGenerator 						generator;


	std::vector<std::vector<AChunk*>> 	tabChunks;
	std::mutex 							tabChunks_mutex;

	glm::ivec2 							position;

	int 								renderDistance;
	int 								generationDistance;
	int 								size_tab;

	
	std::mutex 							&realPlayerPos_mutex;
	glm::vec3 							&realPlayerPos;

	std::mutex 							&playerHasMoved_mutex;
	bool 								&playerHasMoved;
	
	std::mutex 							&to_VAO_mutex;
	std::deque<info_VAO*> 				&to_VAO;
	
	std::mutex 							&toDeleteVAO_mutex;
	std::deque<glm::ivec2> 				&toDeleteVAO;

	std::mutex 							&endThread_mutex;
	bool 								&endThread;



	
	
	void 								deleteBadChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);
	void 								createGoodChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);
	void 								updateChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos);



	bool 								compileChunksWithNeighbours(AChunk *chunk, s_neighbours neighbours);
		
	
	s_neighbours 						getNeighbours(glm::ivec2 tabPos, glm::ivec2 chunkPos);
	
	void 								setChunkNeighbours(AChunk *chunk, s_neighbours neighbours);

	//PARKOUR

	glm::ivec2 incr[4] = {glm::ivec2(1, 0), glm::ivec2(0, 1), glm::ivec2(-1, 0), glm::ivec2(0, -1)};
	u_int incr_pos = 0;


	u_int size_pos = 0;
	u_int size_direction = 0;

	void getNextPos(glm::ivec2 &pos);
	void resetGetNextPos();

	public :

	ChunkInstanciator(u_int renderDistance,
						glm::vec3 &playerPos, std::mutex &playerPos_mutex,
						std::deque<info_VAO*> &to_VAO, std::mutex &to_VAO_mutex,
						std::deque<glm::ivec2> &toDeleteVAO, std::mutex &toDeleteVAO_mutex,
						bool &playerHasMoved, std::mutex &playerHasMoved_mutex,
						bool &windoeShouldClose, std::mutex &windowShouldClose_mutex);

	~ChunkInstanciator();

	std::mutex 							&getTabChunks_mutex();
	std::vector<std::vector<AChunk*>> 	&getTabChunks();

	void update();

};



#endif