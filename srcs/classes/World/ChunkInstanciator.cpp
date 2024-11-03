#include <classes/World/ChunkInstanciator.hpp>

ChunkInstanciator::ChunkInstanciator(u_int renderDistance, glm::vec3 &playerPos, std::mutex &playerPos_mutex,
										std::deque<info_VAO*> &to_VAO, std::mutex &to_VAO_mutex,
										std::deque<glm::ivec2> &toDeleteVAO, std::mutex &toDeleteVAO_mutex)
: renderDistance(renderDistance),
realPlayerPos(playerPos), realPlayerPos_mutex(playerPos_mutex),
to_VAO(to_VAO), to_VAO_mutex(to_VAO_mutex),
toDeleteVAO(toDeleteVAO), toDeleteVAO_mutex(toDeleteVAO_mutex)
{
	generationDistance = renderDistance + 1;
	size_tab = generationDistance * 2 + 1;
	std::cout << "gen dist : "<< generationDistance << std::endl;
	tabChunks.resize((size_tab), std::vector<AChunk*>());

	for (u_int i = 0; i < size_tab; i++)
	{
		tabChunks[i].resize((size_tab), NULL);
	}

	std::srand(std::time(0));
	long int seed;
	seed = 1722331298;
	seed = std::rand();
	std::cout << "seed " << seed << std::endl;
	ChunkGenerator::initNoise(seed);
}

ChunkInstanciator::~ChunkInstanciator()
{
}



int mod_floor(int a, int n) {
	if (a < 0)
		a += (-a / n + 1) * n; 
    return (a % n);
}

int distance(glm::ivec2 a, glm::ivec2 b)
{
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}



bool ChunkInstanciator::compileChunksWithNeighbours(AChunk *chunk, s_neighbours neighbours)
{
	if (!chunk)
		return false;
	glm::ivec2 chunkPos = chunk->getPos();
	// if (neighbours.north && neighbours.east && neighbours.south && neighbours.west)
		// std::cout << "is genereated : " << neighbours.north->getIsGenerated() << " " << neighbours.south->getIsGenerated() << " " << neighbours.east->getIsGenerated() << " " << neighbours.west->getIsGenerated() << std::endl;
	if (!neighbours.north || !neighbours.south || !neighbours.east || !neighbours.west
		|| !neighbours.north->getIsGenerated() || !neighbours.south->getIsGenerated()
		|| !neighbours.east->getIsGenerated() || !neighbours.west->getIsGenerated()
		|| neighbours.north->getPos() != chunkPos + glm::ivec2(0, 1)
		|| neighbours.south->getPos() != chunkPos + glm::ivec2(0, -1)
		|| neighbours.east->getPos() != chunkPos + glm::ivec2(1, 0)
		|| neighbours.west->getPos() != chunkPos + glm::ivec2(-1, 0))
			return false;
	chunk->publicCompile();

	// std::cout << "compiled chunk : " << chunkPos.x << " " << chunkPos.y << " --- ";
	info_VAO *info = new info_VAO();
	*info = {chunk->getPtrVertices(), chunk->getPtrIndices(), chunk->getPos()};
	// std::cout << "pushing to VAO" << std::endl;
	to_VAO_mutex.lock();
	to_VAO.push_back(info);
	to_VAO_mutex.unlock();


	return true;

}

s_neighbours ChunkInstanciator::getNeighbours(glm::ivec2 tabPos, glm::ivec2 chunkPos)
{
	s_neighbours neighbours;
	neighbours.east = tabChunks[(tabPos.x + 1) % size_tab][tabPos.y];
	if (neighbours.east && neighbours.east->getPos().x != chunkPos.x + 1)
		neighbours.east = NULL;
		
	neighbours.west = tabChunks[(tabPos.x - 1 + size_tab) % size_tab][tabPos.y];
	if (neighbours.west && neighbours.west->getPos().x != chunkPos.x - 1)
		neighbours.west = NULL;

	neighbours.north = tabChunks[tabPos.x][(tabPos.y + 1) % size_tab];
	if (neighbours.north && neighbours.north->getPos().y != chunkPos.y + 1)
		neighbours.north = NULL;

	neighbours.south = tabChunks[tabPos.x][(tabPos.y - 1 + size_tab) % size_tab];
	if (neighbours.south && neighbours.south->getPos().y != chunkPos.y - 1)
		neighbours.south = NULL;

	return neighbours;
}

void ChunkInstanciator::setChunkNeighbours(AChunk *chunk, s_neighbours neighbours)
{
	chunk->setNeighbour(NEIGHB_NORTH, neighbours.north);
	if (neighbours.north)
		neighbours.north->setNeighbour(NEIGHB_SOUTH, chunk);
	chunk->setNeighbour(NEIGHB_SOUTH, neighbours.south);
	if (neighbours.south)
		neighbours.south->setNeighbour(NEIGHB_NORTH, chunk);
	chunk->setNeighbour(NEIGHB_EAST, neighbours.east);
	if (neighbours.east)
		neighbours.east->setNeighbour(NEIGHB_WEST, chunk);
	chunk->setNeighbour(NEIGHB_WEST, neighbours.west);
	if (neighbours.west)
		neighbours.west->setNeighbour(NEIGHB_EAST, chunk);
}


void ChunkInstanciator::update()
{

	bool debug = true ;
	while(1)
	{

		realPlayerPos_mutex.lock();
		glm::vec3 playerPos = realPlayerPos;
		realPlayerPos_mutex.unlock();

		glm::ivec2 playerChunkPos = {playerPos.x / AChunk::sizeX, playerPos.z / AChunk::sizeY};
		
		

		glm::ivec2 playerTabPos;
		playerTabPos.x = mod_floor(playerChunkPos.x, size_tab);
		playerTabPos.y = mod_floor(playerChunkPos.y, size_tab);
		


		for (int x = -generationDistance ; x <= generationDistance; x++) {
		for (int y = -generationDistance; y <= generationDistance; y++) {
			

					// glm::ivec2 chunkTabPos((playerTabPos.x + x + size_tab) % size_tab, (playerTabPos.y + y + size_tab) % size_tab);
					glm::ivec2 chunkTabPos(mod_floor(playerTabPos.x + x, size_tab),
											mod_floor(playerTabPos.y + y, size_tab));

					
					glm::ivec2 chunkPos;

					if (tabChunks[chunkTabPos.x][chunkTabPos.y] == NULL)
					{
						chunkPos = glm::ivec2(playerChunkPos.x + x, playerChunkPos.y + y);
						createGoodChunk(chunkPos, chunkTabPos, playerChunkPos);

						continue ;
					}
					chunkPos = tabChunks[chunkTabPos.x][chunkTabPos.y]->getPos();

					if (chunkPos.x != playerChunkPos.x + x || chunkPos.y != playerChunkPos.y + y)
					{
						deleteBadChunk(chunkPos, chunkTabPos, playerChunkPos);
						chunkPos = glm::ivec2(playerChunkPos.x + x, playerChunkPos.y + y);
						createGoodChunk(chunkPos, chunkTabPos, playerChunkPos);

						continue ;
					}

					updateChunk(chunkPos, chunkTabPos, playerChunkPos);

		}
		}

	}
}



void ChunkInstanciator::deleteBadChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled())
	{
		toDeleteVAO_mutex.lock();
		toDeleteVAO.push_back(tabChunks[chunkTabPos.x][chunkTabPos.y]->getPos());
		toDeleteVAO_mutex.unlock();
		tabChunks[chunkTabPos.x][chunkTabPos.y]->setIsCompiled(false);
	}

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsGenerated())
	{
		delete tabChunks[chunkTabPos.x][chunkTabPos.y];
		tabChunks[chunkTabPos.x][chunkTabPos.y] = NULL;
	}
}

void ChunkInstanciator::createGoodChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	
	// int chunk_dist = distance(playerChunkPos, chunkPos);

	// if (chunk_dist > generationDistance)
	// 	return ;
	if (!tabChunks[chunkTabPos.x][chunkTabPos.y] || tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsGenerated() == false)
	{
		tabChunks[chunkTabPos.x][chunkTabPos.y] = new ChunkRLE(chunkPos.x, chunkPos.y, 0);
		setChunkNeighbours(tabChunks[chunkTabPos.x][chunkTabPos.y], getNeighbours(chunkTabPos, chunkPos));

		tabChunks[chunkTabPos.x][chunkTabPos.y]->updateFromRaw(generator.generator(chunkPos));

		s_neighbours neighbours = getNeighbours(chunkTabPos, chunkPos);
		AChunk** neighb = (AChunk**)(&neighbours);
		for (int i = 0; i < 4; i++)
		{
			if (!neighb[i])
				continue ;
			
			glm::ivec2 neighbPos = neighb[i]->getPos();
			glm::ivec2 neighbTabPos;

			neighbTabPos.x = mod_floor(neighbPos.x, size_tab);
			neighbTabPos.y = mod_floor(neighbPos.y, size_tab);


			updateChunk(neighbPos, neighbTabPos, playerChunkPos);
		}

	}
	// if (chunk_dist > renderDistance)
	// 	return ;

	bool didIt = compileChunksWithNeighbours(tabChunks[chunkTabPos.x][chunkTabPos.y], getNeighbours(chunkTabPos, chunkPos));
	
}

void ChunkInstanciator::updateChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	if (!tabChunks[chunkTabPos.x][chunkTabPos.y])
		return ;
	// int chunk_dist = distance(playerChunkPos, chunkPos);

	// bool to_cp = chunk_dist > renderDistance;
	// bool to_dl = chunk_dist <= renderDistance;
	


	bool to_cp = true;
	bool to_dl = (chunkPos.x - playerChunkPos.x > renderDistance || playerChunkPos.x - chunkPos.x < -renderDistance
				|| chunkPos.y - playerChunkPos.y > renderDistance || playerChunkPos.y - chunkPos.y < -renderDistance);

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled() && to_dl)
	{
		toDeleteVAO_mutex.lock();
		toDeleteVAO.push_back(tabChunks[chunkTabPos.x][chunkTabPos.y]->getPos());
		toDeleteVAO_mutex.unlock();
		tabChunks[chunkTabPos.x][chunkTabPos.y]->setIsCompiled(false);

		return ;
	}

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled() == false && GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
	{
		bool didIt = compileChunksWithNeighbours(tabChunks[chunkTabPos.x][chunkTabPos.y], getNeighbours(chunkTabPos, chunkPos));
	}

}
 

