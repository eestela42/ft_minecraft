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
	private:
		t_vertexData *skyVertData;
		VertexArrayObject* VAO;
		std::vector<float> vertices;
		std::vector<u_int> shape;
		std::vector<Texture> texture;
		Shader *shader;

	public:
		CrossHair(Shader *CrossHairShader);
		~CrossHair();
		void	draw();
		

	
};


#endif