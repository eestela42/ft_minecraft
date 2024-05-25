#include <classes/World/Chunk.hpp>
#include <classes/World/PerlinNoise.hpp>
#include <classes/World/ChunkGenerator.hpp>

std::vector<std::vector<Chunk *>> Chunk::loadedChunks;
u_int Chunk::idCount = 0;

Chunk::Chunk() : posX(0), posY(0) {}

Chunk::Chunk(int posX, int posY) : posX(posX), posY(posY)
{
	idCount++;
	id = idCount;
	
}

void Chunk::PublicGenerate()
{
	if (PROFILER_ON)
		Profiler::StartTracking("Chunk::PublicGenerate()");
	ChunkGenerator generator;
	updateFromRaw(generator.generator(*this));
	isGenerated = true;

	if (PROFILER_ON)
		Profiler::StopTracking("Chunk::PublicGenerate()");
}




void Chunk::setRenderDistance(int renderDistance)
{
// 	if (renderDistance == loadedChunks.size())
// 	{
// 		return;
// 	}
// 	std::vector<std::vector<Chunk *>> oldChunks = loadedChunks;

// 	loadedChunks.assign(renderDistance * 2 + 1, std::vector<Chunk *>());
// 	for (int x = 0; x < renderDistance * 2 + 1; x++)
// 	{
// 		loadedChunks[x].resize(renderDistance * 2 + 1, nullptr);
// 		for (int y = 0; y < renderDistance * 2 + 1; y++)
// 			loadedChunks[x][y] = NULL;
// 	}

// 	if (renderDistance > oldChunks.size())
// 	{
// 		for (std::vector<std::vector<Chunk *>>::iterator iterator = oldChunks.begin(); iterator != oldChunks.end(); iterator++)
// 		{
// 			for (std::vector<Chunk *>::iterator iterator2 = iterator->begin(); iterator2 != iterator->end(); iterator2++)
// 			{
// 				(*iterator2)->loadChunk();
// 			}
// 		}
// 	}
}

const std::vector<std::vector<Chunk *>> &Chunk::GetLoadedChunks()
{
	return loadedChunks;
}

int Chunk::GetX()
{
	return posX;
}

int Chunk::GetY()
{
	return posY;
}

t_vertexData &Chunk::GetVertexData()
{
// 	SetReady(false);
// 	didUpdate = false;
// 	return dataStruct;
}

std::vector<u_int> &Chunk::GetShapeAssemblyData()
{
// 	SetReady(false);
// 	didUpdate = false;
// 	return shapeAssemblyData;
}

Chunk::~Chunk()
{
	// UnloadChunk();
	// if (data && (void *)data != (void *)"")
	// {
	// 	std::cout << "Deleting data" << std::endl;
	// 	free(data);
	// 	std::cout << "----Deleting data" << std::endl;
	// }
}