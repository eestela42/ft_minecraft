#include <classes/Texture/TextureLoader.hpp>

bool TextureLoader::isReady = false;

static GLenum channelColor(int nrChannels)
{
	switch (nrChannels)
	{
	case 1:
		return GL_RED;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}
	return GL_RGB;
}

bool TextureLoader::LoadTexture(std::string fileName)
{
	if (!isReady)
		Init();

	int width, height, nrChannels;
	unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	GLenum format = channelColor(nrChannels);

	if (data)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		stbi_image_free(data);
		texture = {textureID, width, height, nrChannels};
		return true;
	}
	else
		std::cout << "Failed to load texture : " << fileName << std::endl;
	stbi_image_free(data);
	return false;
}

bool TextureLoader::LoadTextureArray(std::vector<std::string> fileNames)
{
	if (!isReady)
		Init();
	int width, height, nrChannels;
	unsigned char *data = stbi_load(fileNames[0].c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture : " << fileNames[0] << std::endl;
		stbi_image_free(data);
		return false;
	}
	stbi_image_free(data);

	GLenum format = channelColor(nrChannels);
	uint32_t textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, fileNames.size(), 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	for (unsigned int i = 1; i < fileNames.size(); i++)
	{
		data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture : " << fileNames[i] << std::endl;
			stbi_image_free(data);
			return false;
		}
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	textureArray = {textureID, width, height, nrChannels, (int)fileNames.size()};
	return true;
}

void TextureLoader::Init()
{
	if (!glfwGetCurrentContext())
		std::cout << "TextureLoader cannot be used before a glfw context has been initialized" << std::endl;
	isReady = true;
}

Texture TextureLoader::GetTexture()
{
	return texture;
}

TextureArray TextureLoader::GetTextureArray()
{
	return textureArray;
}
