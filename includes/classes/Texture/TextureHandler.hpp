#pragma once

#include <classes/Texture/Texture.hpp>
#include <classes/Texture/TextureLoader.hpp>
#include <unordered_map>

class TextureHandler
{
public:
	TextureHandler(const char *folderPath);
	~TextureHandler();

	unsigned int getTextureID(std::string key);

	Texture *getTexture(std::string key);

	std::string addTexture(const Texture *texture, std::string key);

private:
	std::unordered_map<std::string, Texture *> textureMap;
};