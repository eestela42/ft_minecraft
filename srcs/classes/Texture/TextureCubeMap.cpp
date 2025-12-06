#include <classes/Texture/TextureCubeMap.hpp>

TextureCubeMap::~TextureCubeMap()
{}

TextureCubeMap::TextureCubeMap()
{}

TextureCubeMap::TextureCubeMap(u_int id) :
	Texture(id, 0, 0, 0)
{}

TextureCubeMap::TextureCubeMap(const TextureCubeMap &other)
{
    id = other.id;
    width = other.width;
    height = other.height;
    nrChannels = other.nrChannels;
}

