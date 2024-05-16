#include <classes/World/Chunk.hpp>
#include <classes/World/PerlinNoise.hpp>
#include <classes/World/ChunkGenerator.hpp>


std::vector<std::vector<Chunk*>> Chunk::loadedChunks;
u_int Chunk::idCount = 0;

Chunk::Chunk() : posX(0), posY(0){}

Chunk::Chunk(int posX, int posY) : posX(posX), posY(posY) {
	idCount++;
	id = idCount;
	loadChunk();
}

void Chunk::PublicGenerate() {
	if (PROFILER_ON)
		Profiler::StartTracking("Chunk::PublicGenerate()");
	isGenerated = true;
	ChunkGenerator generator;

	updateFromRaw(generator.generator(*this));

	isCompiled = false;
	if (PROFILER_ON)
		Profiler::StopTracking("Chunk::PublicGenerate()");
}


void	Chunk::loadChunk() {
	int size = loadedChunks.size();
	if (size) {
		loadedChunks[(posX % size + size) % size][(posY % size + size) % size] = this;
	}
	UpdateNeighbors();
	for (int i = 0; i < 4; i++) {
		if (neighborChunks[i]) {
			neighborChunks[i]->UpdateNeighbors();
		}
	}
}

void	Chunk::UnloadChunk() {
	int size = loadedChunks.size();
	if (size) {
		loadedChunks[(posX % size + size) % size][(posY % size + size) % size] = NULL;
	}
	for (int i = 0; i < 4; i++) {
		if (neighborChunks[i]) {
			neighborChunks[i]->UpdateNeighbors();
		}
	}
}

void	Chunk::setRenderDistance(int renderDistance) {
	if (renderDistance == loadedChunks.size()) {
		return ;
	}
	std::vector<std::vector<Chunk*>> oldChunks = loadedChunks;

	loadedChunks.assign(renderDistance * 2 + 1, std::vector<Chunk*>());
	for (int x = 0; x < renderDistance * 2 + 1; x++)
	{
		loadedChunks[x].resize(renderDistance * 2 + 1, nullptr);
		for (int y = 0; y < renderDistance * 2 + 1; y++)
			loadedChunks[x][y] = NULL;
	}

	if (renderDistance > oldChunks.size()) {
		for (std::vector<std::vector<Chunk*>>::iterator iterator = oldChunks.begin(); iterator != oldChunks.end(); iterator++) {
			for (std::vector<Chunk*>::iterator iterator2 = iterator->begin(); iterator2 != iterator->end(); iterator2++) {
				(*iterator2)->loadChunk();
			}
		}
	}
}

const std::vector<std::vector<Chunk*>> &Chunk::GetLoadedChunks() {
	return loadedChunks;
}

bool Chunk::IsRealNeighbor(int chunkX, int chunkY) {
	if ((std::abs(chunkX - posX) == 1 && chunkY == posY) || (std::abs(chunkY - posY) == 1 && chunkX == posX)) {
		return true;
	}
	return false;
}

Chunk *Chunk::GetNeighbor(int x, int y) {
	int size = loadedChunks.size();
	return loadedChunks[(x % size + size) % size][(y % size + size) % size];
}

void Chunk::UpdateNeighbors() {
	neighborChunks[0] = GetNeighbor(posX, posY + 1);
	u_int neighborID = (neighborChunks[0]) ? neighborChunks[0]->id : 0;
	if (neighborID != neighborChunksID[0]) {
		isCompiled = false;
		didUpdate = true;
	}
	neighborChunksID[0] = neighborID;

	neighborChunks[1] = GetNeighbor(posX + 1, posY);
	neighborID = (neighborChunks[1]) ? neighborChunks[1]->id : 0;
	if (neighborID != neighborChunksID[1]){
		isCompiled = false;
		didUpdate = true;
	}
	neighborChunksID[1] = neighborID;

	neighborChunks[2] = GetNeighbor(posX, posY - 1);
	neighborID = (neighborChunks[2]) ? neighborChunks[2]->id : 0;
	if (neighborID != neighborChunksID[2]){
		isCompiled = false;
		didUpdate = true;
	}
	neighborChunksID[2] = neighborID;

	neighborChunks[3] = GetNeighbor(posX - 1, posY);
	neighborID = (neighborChunks[3]) ? neighborChunks[3]->id : 0;
	if (neighborID != neighborChunksID[3]){
		isCompiled = false;
		didUpdate = true;
	}
	neighborChunksID[3] = neighborID;
}

void Chunk::SetReady(bool isRecursive) {
	if (!isGenerated) {
		PublicGenerate();
	}
	if (!isRecursive && !isCompiled) {
		isCompiled = true;
		for (int i = 0; i < 4; i++) {
			if (neighborChunks[i]) {
				neighborChunks[i]->SetReady(true);
			}
		}
		if (PROFILER_ON)
			Profiler::StartTracking("Chunk::CompileData()");
		CompileData();
		if (PROFILER_ON)
			Profiler::StopTracking("Chunk::CompileData()");
		didUpdate = true;
	}
}

bool Chunk::IsGenerated() {
	return isGenerated;
}

bool Chunk::DidUpdate() {
	return didUpdate;
}

int Chunk::GetX() {
	return posX;
}

int Chunk::GetY() {
	return posY;
}

void Chunk::MakeDirty()
{
	isGenerated = false;
	isCompiled = false;
	didUpdate = true;
}

t_vertexData &Chunk::GetVertexData() {
	SetReady(false);
	didUpdate = false;
	return dataStruct;
}

std::vector<u_int>&		Chunk::GetShapeAssemblyData() {
	SetReady(false);
	didUpdate = false;
	return shapeAssemblyData;
}

Chunk::~Chunk() {
	UnloadChunk();
	if (data && (void*)data != (void*)"") {
		std::cout << "Deleting data" << std::endl;
		free(data);
		std::cout << "----Deleting data" << std::endl;
	}

}