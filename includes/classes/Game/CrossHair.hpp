# ifndef CROSSHAIR_HPP
# define CROSSHAIR_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
# include <stdio.h>

#include <classes/VAO/VertexArrayObjectHandler.hpp>
#include <classes/Texture/Texture.hpp>
#include <string>
#include <stb/stb_image.h>

class CrossHair
{
	public:
		CrossHair(Shader *CrossHairShader);
		~CrossHair();

		void drawCrossHair(glm::mat4 matrix, glm::vec3 pos);
		
		std::vector<unsigned int> indices = {0, 1, 2};
		Shader *shader;

		VertexArrayObject* VAO;
		std::vector<float> vertices;
		std::vector<u_int> shape;
		std::vector<Texture> texture;

		void	draw();
		

	private:
		t_vertexData *skyVertData;
	
};


#endif