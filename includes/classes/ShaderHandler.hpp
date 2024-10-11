#pragma once

#include <classes/Shader.hpp>
#include <unordered_map>

class ShaderHandler
{
	public:
	/** @brief Compiles one shader program per subfolder.*/ 
	~ShaderHandler();
	ShaderHandler(const char* shaderFolderPath);
	/** @brief Activates Shader with the given name.*/ 
	void Use(const char* shaderFolderName);
	Shader *GetShader(const char* shaderFolderName);

	private:
		std::unordered_map<std::string, Shader*> shaderMap;
		// Shader *activeShader = NULL;
};