#include <classes/Texture/TextureLoader.hpp>

bool TextureLoader::isReady = false;

Texture TextureLoader::LoadTexture(std::string fileName) {
    if (!isReady) Init();

    int width, height, nrChannels;
    unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	GLenum format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

    if (data) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
        return Texture(textureID, width, height, nrChannels);
    } else {
        std::cout << "Failed to load texture : " << fileName << std::endl;
        assert(!"TextureLoader::LoadTexture stbi failed to load the texture");
    }
}

TextureArray TextureLoader::LoadTextureArray(std::vector<std::string> fileNames) {
    if (!isReady) Init();

    int width, height, nrChannels;
    unsigned char* data = stbi_load(fileNames[0].c_str(), &width, &height, &nrChannels, 0);
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    if (data) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, fileNames.size(), 0, format, GL_UNSIGNED_BYTE, NULL);
        for (unsigned int i = 1; i < fileNames.size(); i++) {
            data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else {
                std::cout << "Failed to load texture : " << fileNames[i] << std::endl;
                assert(!"TextureLoader::LoadTextureArray stbi failed to load the texture");
            }
        }
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        return TextureArray(textureID, width, height, nrChannels, fileNames.size());
    } else {
        std::cout << "Failed to load texture : " << fileNames[0] << std::endl;
        assert(!"TextureLoader::LoadTextureArray stbi failed to load the texture");
    }
}

void TextureLoader::Init() {
    if (!glfwGetCurrentContext()) {
        std::cout << "TextureLoader cannot be used before a glfw context has been initialized" << std::endl;
        assert(!"TextureLoader::Init() glfwGetCurrentContext failed to load the current context");
    }
    stbi_set_flip_vertically_on_load(true);
    isReady = true;
}