#ifndef CHUNKCLASSIC_HPP
#define CHUNKCLASSIC_HPP

#include <classes/World/AChunk.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <classes/types.hpp>



class ChunkClassic : public AChunk {

private :
	std::vector<Vertex> 		vertices;
	std::vector<unsigned int> 	indices;

public:
	ChunkClassic(int x, int y, int z);
	~ChunkClassic();

	void updateFromRaw(u_char *data) override;
	void publicCompile() override;




	u_int VBO, EBO, VAO;

	void	createFace(int x, int y, int z,  int tab_x, int tab_y, int tab_z, int dir);
	void	setupMesh();
	
	
	t_vbo_data 					getPtrVertices() override;
	std::vector<unsigned int>*	getPtrIndices() override;


};





#endif