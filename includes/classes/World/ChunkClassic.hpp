#ifndef CHUNKCLASSIC_HPP
#define CHUNKCLASSIC_HPP

#include <classes/World/AChunk.hpp>
#include <glm/glm.hpp>
# include <glad/glad.h>
# include <GLFW/glfw3.h>

#include <classes/types.hpp>


class ChunkClassic : public AChunk {

public:
	ChunkClassic(int x, int y, int z);
	~ChunkClassic();

	void publicGenerate() override;
	void updateFromRaw(u_char *data) override;
	void publicCompile() override;



	u_int VBO, EBO, VAO;

	void	createFace(int x, int y, int z,  int tab_x, int tab_y, int tab_z, int dir);
	void	setupMesh();
	void	draw() override ;


};





#endif