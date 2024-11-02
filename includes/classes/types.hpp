#ifndef TYPES_HPP
#define TYPES_HPP

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3   Position;
	int 		light;
};

typedef struct t_vbo_data
{
	void *data = NULL;
	u_int size = 0;
};

struct info_VAO
{
	t_vbo_data					vertices;
	std::vector<unsigned int>*	indices;
	glm::ivec2 					pos;
};

struct VAO_data
{
	void*						vertices;
	std::vector<unsigned int>*	indices;
	u_int 						VAO;
};


#endif