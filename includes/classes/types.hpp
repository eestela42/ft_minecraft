#ifndef TYPES_HPP
#define TYPES_HPP

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3   Position;
	int 		light;
};

struct info_VAO
{
	std::vector<Vertex>*		vertices;
	std::vector<unsigned int>*	indices;
	glm::ivec2 					pos;
};


#endif