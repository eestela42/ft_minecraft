#include <classes/VertexBufferObject.hpp>
#include <iostream>

VertexBufferObject::VertexBufferObject(t_vertexData& vertexData){
	verticesSize = vertexData.size;
    glGenBuffers(1, &index);
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertexData.data, GL_STATIC_DRAW);
}

void VertexBufferObject::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, index);
}

void VertexBufferObject::DeleteBuffers() {
	glDeleteBuffers(1, &index);
}

int VertexBufferObject::GetSize() const {
    return verticesSize;
}