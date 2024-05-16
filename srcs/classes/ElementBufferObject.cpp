#include <classes/ElementBufferObject.hpp>
#include <iostream>

// Parameterized constructor
ElementBufferObject::ElementBufferObject(const std::vector<unsigned int> &indices)
{
	indicesSize = indices.size();
	indicesDataSize = indicesSize * sizeof(indices[0]);
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesDataSize, indices.data(), GL_STATIC_DRAW);
}

void ElementBufferObject::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
}

void ElementBufferObject::DeleteBuffers()
{
	glDeleteBuffers(1, &index);
}

std::size_t ElementBufferObject::GetSize() const
{
	return indicesSize;
}

std::size_t ElementBufferObject::GetDataSize() const
{
	return indicesDataSize;
}