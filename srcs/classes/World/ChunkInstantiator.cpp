#include <classes/World/ChunkInstantiator.hpp>

bool isInCircle(long int x, long int y, long int radius, long int circleX, long int circleY)
{
	if ((x - circleX) * (x - circleX) + (y - circleY) * (y - circleY) <= radius * radius)
	{
		return true;
	}
	return false;
}

ChunkInstantiator::~ChunkInstantiator()
{
	const std::vector<std::vector<Chunk *>> &loadedChunks = Chunk::GetLoadedChunks();

	for (unsigned long i = 0; i < loadedChunks.size(); i++)
	{
		for (unsigned long y = 0; y < loadedChunks[i].size(); y++)
		{
			delete loadedChunks[i][y];
		}
	}
}

ChunkInstantiator::ChunkInstantiator(VertexArrayObjectHandler *vertexArrayObjectHandler, int renderDistance, ShaderHandler *shaderHandler)
{
	// updateGen("generation");

	this->shaderHandler = shaderHandler;

	playerChunkPosX = 0;
	playerChunkPosY = 0;
	Shader *shader = shaderHandler->GetShader(ChunkRLE::shaderName);
	bool showChunkDebug = false;
	this->vertexArrayObjectHandler = vertexArrayObjectHandler;
	this->renderDistance = renderDistance;
	// int chunkLoadingSize = Chunk::GetLoadedChunks().size();
	std::vector<Chunk *> chunks;

	// get an random long int

	std::srand(std::time(0));
	long int seed;
	seed = 1722331298;
	seed = std::rand();
	std::cout << ChunkRLE::shaderName << std::endl;
	std::cout << "seed " << seed << std::endl;
	ChunkGenerator::initNoise(seed);

}

void ChunkInstantiator::Update(glm::vec3 *addrPlayerPos, std::vector<Chunk *> **adrrStableState,
	std::mutex &cameraMutex, std::mutex &stableMutex)
{
while(1)
{
	Shader *shader = shaderHandler->GetShader(ChunkRLE::shaderName);
	const std::vector<std::vector<Chunk *>> &loadedChunks = Chunk::GetLoadedChunks();
	int size = loadedChunks.size();

	cameraMutex.lock();
	glm::vec3 playerPos = *addrPlayerPos;
	cameraMutex.unlock();
	playerPos.x /= Chunk::sizeX; // ChunkSize
	playerPos.z /= Chunk::sizeY;

	int oldPlayerChunkPosX = playerChunkPosX;
	int oldPlayerChunkPosY = playerChunkPosY;
	playerChunkPosX = playerPos.x;
	playerChunkPosY = playerPos.z;

	std::vector<Chunk*> *newStableState = new std::vector<Chunk*>();

	for (int x = playerChunkPosX - size/2; x <= playerChunkPosX + size/2; x++){
	for (int y = playerChunkPosY - size/2; y <= playerChunkPosX + size/2; y++)
	{
		int p_x = x % size; //pos dans loadedChunks
		int p_y = y % size;
		if (!loadedChunks[p_x][p_y])
		{
			//create new chunk
			continue ;
		}
		if (loadedChunks[p_x][p_y]->GetX() != x || loadedChunks[p_x][p_y]->GetY() != y)
		{
			toDelete.push_back(loadedChunks[p_x][p_y]);
			//loadedChunks[p_x][p_y] = NULL; ?
			continue ;
		}
		
	}
	}



}
	
}