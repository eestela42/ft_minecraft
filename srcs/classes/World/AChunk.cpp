#include <classes/World/AChunk.hpp>

AChunk::AChunk(int x, int y, int z) : posX(x), posY(y), posZ(z)
{	
}

AChunk::~AChunk()
{
	if (data)
	{
		free(data);
	}
}

std::vector<Vertex> *AChunk::getPtrVertices()
{
	printf("GETTER addr vertices : %p\n", &vertices[0]);
	return &vertices;
}

std::vector<unsigned int> *AChunk::getPtrIndices()
{
	printf("GETTER addr indices : %p\n", &indices[0]);
	return &indices;
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

