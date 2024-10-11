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

int distance(glm::ivec2 a, glm::ivec2 b)
{
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}


void ChunkInstanciator::update()
{

	bool debug = true ;
	glm::vec3 playerPos(53553, 6546, 666);
	while(1)
	{

		realPlayerPos_mutex.lock();
		glm::vec3 playerPos = realPlayerPos;
		realPlayerPos_mutex.unlock();

		glm::ivec2 playerChunkPos = {playerPos.x / AChunk::sizeX, playerPos.z / AChunk::sizeY};
		

		glm::ivec2 playerTabPos((playerChunkPos.x + size_tab) % (size_tab), (playerChunkPos.y + size_tab) % (size_tab));


		for (int x = -generationDistance ; x <= generationDistance; x++) {
		for (int y = -generationDistance; y <= generationDistance; y++) { 
					glm::ivec2 chunkTabPos((playerTabPos.x + x + size_tab) % size_tab, (playerTabPos.y + y + size_tab) % size_tab);
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
	int chunk_dist = distance(playerChunkPos, chunkPos);
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
	
	int chunk_dist = distance(playerChunkPos, chunkPos);

	if (chunk_dist > generationDistance)
		return ;
	if (!tabChunks[chunkTabPos.x][chunkTabPos.y] || tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsGenerated() == false)
	{
		tabChunks[chunkTabPos.x][chunkTabPos.y] = new ChunkClassic(chunkPos.x, chunkPos.y, 0);
		// tabChunks[tabX][tabY]->publicGenerate();
		tabChunks[chunkTabPos.x][chunkTabPos.y]->updateFromRaw(generator.generator(chunkPos));
	}
	if (chunk_dist > renderDistance)
		return ;

	tabChunks[chunkTabPos.x][chunkTabPos.y]->publicCompile();

	
	info_VAO *info = new info_VAO();
	*info = {&tabChunks[chunkTabPos.x][chunkTabPos.y]->vertices, &tabChunks[chunkTabPos.x][chunkTabPos.y]->indices, chunkPos};
	to_VAO_mutex.lock();
	to_VAO.push_back(info);
	to_VAO_mutex.unlock();
}


void ChunkInstanciator::updateChunk(glm::ivec2 chunkPos, glm::ivec2 chunkTabPos, glm::ivec2 playerChunkPos)
{
	int chunk_dist = distance(playerChunkPos, chunkPos);

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled() && chunk_dist > renderDistance)
	{
		toDeleteVAO_mutex.lock();
		toDeleteVAO.push_back(tabChunks[chunkTabPos.x][chunkTabPos.y]->getPos());
		toDeleteVAO_mutex.unlock();
		tabChunks[chunkTabPos.x][chunkTabPos.y]->setIsCompiled(false);
		return ;
	}

	if (tabChunks[chunkTabPos.x][chunkTabPos.y]->getIsCompiled() == false && chunk_dist <= renderDistance)
	{
		tabChunks[chunkTabPos.x][chunkTabPos.y]->publicCompile();
		info_VAO *info = new info_VAO();
		*info = {&tabChunks[chunkTabPos.x][chunkTabPos.y]->vertices, &tabChunks[chunkTabPos.x][chunkTabPos.y]->indices, chunkPos};
		to_VAO_mutex.lock();
		to_VAO.push_back(info);
		to_VAO_mutex.unlock();
	}
}
 

