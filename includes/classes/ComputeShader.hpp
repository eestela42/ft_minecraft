
#ifndef COMPUTESHADER_HPP
#define COMPUTESHADER_HPP

#include <classes/Shader.hpp>


class ComputeShader : public Shader
{
	public:
		ComputeShader(const std::string &folderPath);
		// void Dispatch(int x, int y, int z);
};

#endif