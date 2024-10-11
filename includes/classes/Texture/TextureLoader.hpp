#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <classes/Texture/Texture.hpp>
#include <classes/Texture/TextureArray.hpp>
#include <stb/stb_image.h>
#include <assert.h>
#include <vector>

class TextureLoader
{
    public:
        static Texture LoadTexture(std::string fileName);
        static TextureArray LoadTextureArray(std::vector<std::string> fileNames);

    private:
        static bool isReady;
        static void Init();
};

#endif