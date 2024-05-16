#pragma once

#include <vector>
#include <glad/glad.h>

class ElementBufferObject
{
public:
	ElementBufferObject(const std::vector<unsigned int> &indices);

	void Bind();
	void DeleteBuffers();
	std::size_t GetSize() const;
	std::size_t GetDataSize() const;

private:
	unsigned int index;
	std::size_t indicesSize;
	std::size_t indicesDataSize;
};