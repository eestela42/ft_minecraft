#include <classes/World/AChunk.hpp>

AChunk::AChunk(int x, int y, int z) : posX(x), posY(y), posZ(z)
{}

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


	if (data)
	{
		free(data);
	}
}

glm::ivec2 AChunk::getPos()
{
	return {posX, posY};
}

void AChunk::setData(u_char *data)
{
	this->data = data;
}

u_char *AChunk::getData()
{
	return data;
}


void AChunk::setIsGenerated(bool isGenerated)
{
	this->isGenerated = isGenerated;
}

bool AChunk::getIsGenerated()
{
	return isGenerated;
}

void AChunk::setIsCompiled(bool isCompiled)
{
	this->isCompiled = isCompiled;
}

bool AChunk::getIsCompiled()
{
	return isCompiled;
}

void AChunk::setToUpdate(bool toUpdate)
{
	this->toUpdate = toUpdate;
}

bool AChunk::getToUpdate()
{
	return toUpdate;
}

bool AChunk::isFilled(int x, int y, int z)
{
	return 0;
}

u_char AChunk::blockType(int x, int y, int z)
{
	return 0;
}


bool AChunk::getIsGenerated() const
{
	return isGenerated;
}

bool AChunk::getIsCompiled() const
{
	return isCompiled;
}

bool AChunk::getIsToUpdate() const
{
	return toUpdate;
}

s_neighbours AChunk::getNeighbours() const
{
	return neighbours;
}

void AChunk::setNeighbours(s_neighbours neighbours)
{
	this->neighbours = neighbours;
}

void AChunk::setNeighbour(int direction, AChunk *chunk)
{
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
}

