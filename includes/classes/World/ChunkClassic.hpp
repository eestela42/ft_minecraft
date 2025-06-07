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
	void deleter() override;

	void privGenerate(u_char *data) override;
	void privCompile() override;
	bool privChangeBlock(int x, int y, int z, u_char type) override;

	t_vbo_data 					privGetPtrVertices() override;
	std::vector<unsigned int>*	privGetPtrIndices() override;

	bool						privIsFilled(int x, int y, int z) override;
	u_char						privBlockType(int x, int y, int z) override;



	u_int VBO, EBO, VAO;

	void	createFace(int x, int y, int z,  int tab_x, int tab_y, int tab_z, int dir);
	

};





#endif