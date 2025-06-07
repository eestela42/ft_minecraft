#include <classes/World/AChunk.hpp>

int mod_floor(int a, int n) {
	if (a < 0)
		a += (-a / n + 1) * n; 
    return (a % n);
}

AChunk::AChunk(int x, int y, int z) : posX(x), posY(y), posZ(z)
{
}

AChunk::~AChunk()
{
	if (neighbours.north)
		neighbours.north->setNeighbour(NEIGHB_SOUTH, NULL);
	if (neighbours.south)	
		neighbours.south->setNeighbour(NEIGHB_NORTH, NULL);
	if (neighbours.east)
		neighbours.east->setNeighbour(NEIGHB_WEST, NULL);
	if (neighbours.west)
		neighbours.west->setNeighbour(NEIGHB_EAST, NULL);

	mutex.unlock();
}



/*Public func*/
void AChunk::pubGenerate(u_char *data)
{
	if (PROFILER_ON)
		Profiler::StartTracking("Chunk Generate");
	mutex.lock();
	privGenerate(data);
	isGenerated = true;
	mutex.unlock();
	if (PROFILER_ON)
		Profiler::StopTracking("Chunk Generate");
}

void AChunk::pubCompile()
{
	if (PROFILER_ON)
		Profiler::StartTracking("Chunk Compile");
	mutex.lock();
	privCompile();
	isCompiled = true;
	mutex.unlock();
	if (PROFILER_ON)
		Profiler::StopTracking("Chunk Compile");
}

t_vbo_data AChunk::pubGetPtrVertices()
{
	mutex.lock();
	t_vbo_data ret = privGetPtrVertices();
	mutex.unlock();
	return ret;
}

std::vector<unsigned int>* AChunk::pubGetPtrIndices()
{
	mutex.lock();
	std::vector<unsigned int>* ret = privGetPtrIndices();
	mutex.unlock();
	return ret;
}

bool AChunk::pubIsFilled(int x, int y, int z)
{
	mutex.lock();
	bool ret = privIsFilled(x, y, z);
	mutex.unlock();
	return ret;
}

u_char AChunk::pubBlockType(int x, int y, int z)
{
	mutex.lock();
	u_char ret = privBlockType(x, y, z);
	mutex.unlock();
	return ret;
}

bool AChunk::pubChangeBlock(int x, int y, int z, u_char type)
{
	std::cout << "pubChangeBlock x : " << std::endl;
	mutex.lock();
	if (!isGenerated)
	{
		mutex.unlock();
		return false;
	}
	bool ret = false;
	ret = privChangeBlock(x, y, z, type);
	toUpdate = true;
	mutex.unlock();
	return ret;
}

/*Normal func*/

void AChunk::borrow()
{
	mutex.lock();
	sharedHolder++;
	mutex.unlock();
}


glm::ivec2 AChunk::getPos()
{
	return {posX, posY};
}

void AChunk::setData(u_char *data)
{
	this->data = data;
}

void AChunk::setDataMutex(u_char *data)
{
	mutex.lock();
	this->data = data;
	mutex.unlock();
}

u_char *AChunk::getData()
{
	return data;
}

u_char *AChunk::getDataMutex()
{
	mutex.lock();
	return data;
	mutex.unlock();
}


void AChunk::setIsGenerated(bool isGenerated)
{
	mutex.lock();
	this->isGenerated = isGenerated;
	mutex.unlock();
	
}

bool AChunk::getIsGenerated()
{
	bool ret = isGenerated;
	return ret;
}

bool AChunk::getIsGeneratedMutex()
{
	mutex.lock();
	bool ret = isGenerated;
	mutex.unlock();
	return ret;
}

void AChunk::setIsCompiled(bool isCompiled)
{
	mutex.lock();
	this->isCompiled = isCompiled;
	mutex.unlock();
}

bool AChunk::getIsCompiled()
{
	mutex.lock();
	bool ret = isCompiled;
	mutex.unlock();
	return ret;
}

void AChunk::setToUpdate(bool toUpdate)
{
	mutex.lock();
	this->toUpdate = toUpdate;
	mutex.unlock();
}

bool AChunk::getToUpdate()
{
	mutex.lock();
	bool ret = toUpdate;
	mutex.unlock();
	return ret;
}

s_neighbours AChunk::getNeighbours()
{
	// mutex.lock();
	s_neighbours ret =  neighbours;
	// mutex.unlock();
	return ret;
}

void AChunk::setNeighbours(s_neighbours neighbours)
{
	mutex.lock();
	this->neighbours = neighbours;
	mutex.unlock();
}

void AChunk::setNeighbour(int direction, AChunk *chunk)
{
	mutex.lock();
	switch (direction)
	{
	case NEIGHB_NORTH:
		neighbours.north = chunk;
		break;

	case NEIGHB_SOUTH:
		neighbours.south = chunk;
		break;

	case NEIGHB_EAST:
		neighbours.east = chunk;
		break;
	
	case NEIGHB_WEST:
		neighbours.west = chunk;
		break;
	}
	mutex.unlock();
}

