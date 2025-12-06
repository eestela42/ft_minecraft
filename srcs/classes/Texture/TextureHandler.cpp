#include <classes/Texture/TextureHandler.hpp>
#include <filesystem>

TextureHandler::TextureHandler(const char *folderPath)
{
	// for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
	// 	if (entry.is_regular_file()) {
	// 		const std::string& filePath = entry.path().string();
	// 		Texture* texture = new Texture(TextureLoader::LoadTexture(filePath));
	// 		textureMap[texture->id] = texture;
	// 	}
	// }

	TextureArray blockTextureArray = TextureLoader::LoadTextureArray(
		{
			std::filesystem::path("textures/texturePack/dirt.jpg"),
			std::filesystem::path("textures/texturePack/dirt.jpg"),
			std::filesystem::path("textures/texturePack/grass.jpg"),
			std::filesystem::path("textures/texturePack/stone.jpg"),
			std::filesystem::path("textures/texturePack/sand.jpg"),
			std::filesystem::path("textures/texturePack/oak_wood_side.jpg"),
			std::filesystem::path("textures/texturePack/leaves_2.jpg"),
			std::filesystem::path("textures/texturePack/bedrock.jpg"),
			std::filesystem::path("textures/texturePack/water.jpg"),
			std::filesystem::path("textures/texturePack/snow.jpg"),
			std::filesystem::path("textures/texturePack/iron_ore.jpg"),
			std::filesystem::path("textures/texturePack/gold_ore.jpg"),
			std::filesystem::path("textures/texturePack/diamond_ore.jpg"),
			std::filesystem::path("textures/texturePack/UNKNOWN.jpg"),
		});
	textureMap["blockTextureArray"] = (Texture *)new TextureArray(blockTextureArray);

	std::vector<std::string> faces{
		std::filesystem::path("textures/skybox/right.jpg"),
		std::filesystem::path("textures/skybox/left.jpg"),
		std::filesystem::path("textures/skybox/bottom.jpg"),
		std::filesystem::path("textures/skybox/top.jpg"),
		std::filesystem::path("textures/skybox/front.jpg"),
		std::filesystem::path("textures/skybox/back.jpg")};
	textureMap["skyboxTextureCubeMap"] = (Texture *)new TextureCubeMap(TextureLoader::LoadTextureCubeMap(faces));
}

TextureHandler::~TextureHandler()
{
	for (auto &texture : textureMap)
	{
		delete texture.second;
	}
}

unsigned int TextureHandler::getTextureID(std::string key)
{
	return textureMap[key]->id;
}

Texture *TextureHandler::getTexture(std::string key)
{
	return textureMap[key];
}

std::string TextureHandler::addTexture(const Texture *texture, std::string key)
{
	textureMap[key] = new Texture(*texture);
	return key;
}
