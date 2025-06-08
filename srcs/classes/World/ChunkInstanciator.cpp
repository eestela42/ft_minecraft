#include <classes/World/ChunkInstanciator.hpp>

ChunkInstanciator::ChunkInstanciator(u_int renderDistance,
										glm::vec3 &playerPos,					std::mutex &playerPos_mutex,
										std::deque<info_VAO*> &to_VAO, 			std::mutex &to_VAO_mutex,
										std::deque<glm::ivec2> &toDeleteVAO, 	std::mutex &toDeleteVAO_mutex,
										bool &playerHasMoved, 					std::mutex &playerHasMoved_mutex,
										bool &endThread, 						std::mutex &endThread_mutex,
										bool& casse_block,
										int &displayDistance)
	: renderDistance(renderDistance),
		realPlayerPos(playerPos), realPlayerPos_mutex(playerPos_mutex),
		to_VAO(to_VAO), to_VAO_mutex(to_VAO_mutex),
		toDeleteVAO(toDeleteVAO), toDeleteVAO_mutex(toDeleteVAO_mutex),
		playerHasMoved(playerHasMoved), playerHasMoved_mutex(playerHasMoved_mutex),
		endThread(endThread), endThread_mutex(endThread_mutex),
		casse_block(casse_block),
		displayDistance(displayDistance)
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

	_currentSeed = 1722331298;
	_currentSeed = std::rand();
	std::cout << "seed " << _currentSeed << std::endl;
	ChunkGenerator::initNoise(_currentSeed);
}

ChunkInstanciator::~ChunkInstanciator()
{
}

std::mutex &ChunkInstanciator::getTabChunks_mutex()
{
	return tabChunks_mutex;
}

std::vector<std::vector<AChunk*>> &ChunkInstanciator::getTabChunks()
{
	return tabChunks;
}



int distance(glm::ivec2 a, glm::ivec2 b)
{
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

bool ChunkInstanciator::compileChunksWithNeighbours(AChunk *chunk, s_neighbours neighbours)
{
	if (!chunk) //not supposed to happen curently
		return false;
	glm::ivec2 chunkPos = chunk->getPos();

	if (!neighbours.north || !neighbours.south || !neighbours.east || !neighbours.west
		|| !neighbours.north->getIsGeneratedMutex() || !neighbours.south->getIsGeneratedMutex()
		|| !neighbours.east->getIsGeneratedMutex() || !neighbours.west->getIsGeneratedMutex()
		|| neighbours.north->getPos() != chunkPos + glm::ivec2(0, 1)
		|| neighbours.south->getPos() != chunkPos + glm::ivec2(0, -1)
		|| neighbours.east->getPos() != chunkPos + glm::ivec2(1, 0)
		|| neighbours.west->getPos() != chunkPos + glm::ivec2(-1, 0))
			return false;
	chunk->pubCompile();

	info_VAO *info = new info_VAO();
	*info = {chunk->pubGetPtrVertices(), chunk->pubGetPtrIndices(), chunk->getPos()};

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

void ChunkInstanciator::resetGetNextPos()
{
	size_direction = 0;
	size_pos = 0;
	incr_pos = 0;
}

int ChunkInstanciator::getNextPos(glm::ivec2 &pos)
{
	if (size_direction == 0)
	{
		size_direction++;
		return size_direction;
	}

	pos += incr[incr_pos];
	size_pos++;
	if (size_pos == size_direction)
	{
		size_pos = 0;
		size_direction += incr_pos % 2;
		incr_pos = (incr_pos + 1) % 4;
	}
	return size_direction;
	
}

void ChunkInstanciator::deleteBadChunk(glm::ivec2 chunkTabPos)
{
	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled())
	{
		toDeleteVAO_mutex.lock();

		toDeleteVAO.push_back(tabChunks[chunkTabPos.x][chunkTabPos.y]->getPos());

		toDeleteVAO_mutex.unlock();

		tabChunks[chunkTabPos.x][chunkTabPos.y]->setIsCompiled(false);
	}

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsGeneratedMutex())
	{
		tabChunks_mutex.lock();
		tabChunks[chunkTabPos.x][chunkTabPos.y]->deleter();	
		tabChunks[chunkTabPos.x][chunkTabPos.y] = NULL;
		tabChunks_mutex.unlock();

	}
}

void ChunkInstanciator::createGoodChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	
	if (!tabChunks[chunkTabPos.x][chunkTabPos.y] || tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsGeneratedMutex() == false)
	{
		tabChunks[chunkTabPos.x][chunkTabPos.y] = new ChunkRLE(chunkPos.x, chunkPos.y, 0);
		setChunkNeighbours(tabChunks[chunkTabPos.x][chunkTabPos.y], getNeighbours(chunkTabPos, chunkPos));
		tabChunks[chunkTabPos.x][chunkTabPos.y]->pubGenerate(generator.generator(chunkPos));

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

	bool didIt = compileChunksWithNeighbours(tabChunks[chunkTabPos.x][chunkTabPos.y], getNeighbours(chunkTabPos, chunkPos));
	
}

void ChunkInstanciator::updateChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	AChunk *chunk = tabChunks[chunkTabPos.x][chunkTabPos.y];
	if (!chunk) //npt supposed to happen curently
		return ;
	bool to_delete = (chunkPos.x - playerChunkPos.x > renderDistance || playerChunkPos.x - chunkPos.x < -renderDistance
				|| chunkPos.y - playerChunkPos.y > renderDistance || playerChunkPos.y - chunkPos.y < -renderDistance);
	if (to_delete && chunk->getIsCompiled())
	{
		toDeleteVAO_mutex.lock();

		toDeleteVAO.push_back(chunk->getPos());

		toDeleteVAO_mutex.unlock();

		chunk->setIsCompiled(false);
	}
	else if (!to_delete && chunk->getIsCompiled() == false)
	{
		compileChunksWithNeighbours(chunk, getNeighbours(chunkTabPos, chunkPos));
	}
	else if (!to_delete && chunk->getToUpdate())
	{
		if (chunk->getIsCompiled())
		{
			toDeleteVAO_mutex.lock();

			toDeleteVAO.push_back(chunk->getPos());

			toDeleteVAO_mutex.unlock();
			chunk->setIsCompiled(false);
		}

		chunk->setToUpdate(false);

		compileChunksWithNeighbours(chunk, getNeighbours(chunkTabPos, chunkPos));

	}

}


 
int t_x = 0;
int t_y = 0;
int t_z = 1;
void ChunkInstanciator::update()
{
	bool debug = true ;
	glm::ivec2 pos = {0, 0};
	endThread_mutex.lock();
	while(endThread == false)
	{
		endThread_mutex.unlock();
		realPlayerPos_mutex.lock();
		glm::vec3 playerPos = realPlayerPos;
		realPlayerPos_mutex.unlock();

		glm::ivec2 playerChunkPos = {playerPos.x / AChunk::sizeX, playerPos.z / AChunk::sizeY};
		
		glm::ivec2 playerTabPos;
		playerTabPos.x = mod_floor(playerChunkPos.x, size_tab);
		playerTabPos.y = mod_floor(playerChunkPos.y, size_tab);
		

		glm::ivec2 zop = {0, 0};
		resetGetNextPos();
		_updateMutex.lock();
		int size_direction = 0;
		bool sizeDisplayable = true;
		int actualDisplayDistance = 0;
		//temp
		displayDistance = renderDistance;
		for (int x1 = -generationDistance ; getKeepUpdating() && x1 <= generationDistance; x1++) {
		for (int y1 = -generationDistance; getKeepUpdating() && y1 <= generationDistance; y1++) {

			playerHasMoved_mutex.lock();
			if (playerHasMoved)
			{
				playerHasMoved_mutex.unlock();
				break ;
			}
			playerHasMoved_mutex.unlock();

			getNextPos(zop);
			int x = zop.x;
			int y = zop.y;

			glm::ivec2 chunkTabPos(mod_floor(playerTabPos.x + x, size_tab),
									mod_floor(playerTabPos.y + y, size_tab));

			
			glm::ivec2 chunkPos;
			AChunk *chunk = tabChunks[chunkTabPos.x][chunkTabPos.y];

			if (chunk == NULL)
			{
				chunkPos = glm::ivec2(playerChunkPos.x + x, playerChunkPos.y + y);
				createGoodChunk(chunkPos, chunkTabPos, playerChunkPos);

				continue ;
			}
			chunkPos = chunk->getPos();
			
			updateChunk(chunkPos, chunkTabPos, playerChunkPos);


				if (chunkPos.x != playerChunkPos.x + x || chunkPos.y != playerChunkPos.y + y)
				{	//Updatechunk fait deja le boulot nn ?
					deleteBadChunk(chunkTabPos);
					chunkPos = glm::ivec2(playerChunkPos.x + x, playerChunkPos.y + y);
					createGoodChunk(chunkPos, chunkTabPos, playerChunkPos);
				}
				if (!tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled())
					sizeDisplayable = false;

		}
			playerHasMoved_mutex.lock();
			if (playerHasMoved)
			{
				playerHasMoved = false;
				playerHasMoved_mutex.unlock();
				break ;
			}
			playerHasMoved_mutex.unlock();
		}
		_updateMutex.unlock();
		endThread_mutex.lock();
		// displayDistance = actualDisplayDistance;
	}
	endThread_mutex.unlock();
}

long int ChunkInstanciator::getCurrentSeed() const
{
	return _currentSeed;
}

void ChunkInstanciator::changeSeed(long int seed)
{
	std::cout << "Changing seed to " << seed << std::endl;
	if (seed == _currentSeed)
		return ;
	setKeepUpdating(false); 
	
	_currentSeed = seed;
	ChunkGenerator::initNoise(_currentSeed);
	_updateMutex.lock();
	deleteAllChunks();
	setKeepUpdating(true);

	_updateMutex.unlock();
}


void ChunkInstanciator::changeRenderDistance(int newRenderDistance)
{
	if (newRenderDistance == renderDistance)
		return ;
		
	setKeepUpdating(false);
	_updateMutex.lock();
	deleteAllChunks();
	renderDistance = newRenderDistance;
	generationDistance = renderDistance + 1;
	size_tab = generationDistance * 2 + 1;

	tabChunks.clear();
	tabChunks.resize((size_tab), std::vector<AChunk*>());

	for (u_int i = 0; i < size_tab; i++)
	{
		tabChunks[i].resize((size_tab), NULL);
	}
	setKeepUpdating(true);

	_updateMutex.unlock();
}

void ChunkInstanciator::deleteAllChunks()
{
	to_VAO_mutex.lock();

	to_VAO.clear();

	to_VAO_mutex.unlock();

	for (int i = 0; i < size_tab; i++)
	{
		for (int j = 0; j < size_tab; j++)
		{
			if (tabChunks[i][j])
			{
				deleteBadChunk(glm::ivec2(i, j));
			}
		}
	}
}


void ChunkInstanciator::setKeepUpdating(bool status)
{
	_keepUpdating_mutex.lock();
	_keepupdating = status;
	_keepUpdating_mutex.unlock();
}

bool ChunkInstanciator::getKeepUpdating()
{
	bool ret;
	_keepUpdating_mutex.lock();
	ret = _keepupdating;
	_keepUpdating_mutex.unlock();
	return ret;
}
