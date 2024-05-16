#ifndef __TEXTURELOADER_H__
#define __TEXTURELOADER_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Texture.hpp"
#include "TextureArray.hpp"

#include <stb/stb_image.h>

#include <iostream>
#include <assert.h>
#include <vector>

class TextureLoader
{
public:
	bool LoadTexture(std::string fileName);
	bool LoadTextureArray(std::vector<std::string> fileNames);
	Texture GetTexture();
	TextureArray GetTextureArray();

private:
	Texture texture;
	TextureArray textureArray;

	static bool isReady;
	static void Init();
};

#endif
