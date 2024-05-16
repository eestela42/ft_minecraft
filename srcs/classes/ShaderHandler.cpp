#include <classes/ShaderHandler.hpp>

ShaderHandler::~ShaderHandler()
{
	for (auto &shader : shaderMap)
	{
		delete shader.second;
	}
}

ShaderHandler::ShaderHandler(const char *folderPath)
{
	for (const auto &entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_directory())
		{
			const std::string &filePath = entry.path().string();
			const std::string &fileName = entry.path().filename().string();
			shaderMap[fileName] = new Shader(filePath);
		}
	}
}

void ShaderHandler::Use(const char *shaderFolderName)
{
	Shader *shader = shaderMap[shaderFolderName];

	if (shader)
	{
		shader->Use();
	}
	else
	{
		assert(!"ShaderHandler::Use shader doesn't exist");
	}
}

Shader *ShaderHandler::GetShader(const char *shaderFolderName)
{
	Shader *shader = shaderMap[shaderFolderName];
	if (!shader)
	{
		assert(!"ShaderHandler::GetShader shader doesn't exist");
	}
	return shader;
}