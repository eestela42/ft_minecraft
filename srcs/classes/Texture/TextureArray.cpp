#include <classes/Texture/TextureArray.hpp>

TextureArray::~TextureArray() {}

TextureArray::TextureArray() {}

TextureArray::TextureArray(u_int id, int width, int height, int nrChannels, int depth)
	: id(id), width(width), height(height), nrChannels(nrChannels), depth(depth) {}

TextureArray::TextureArray(const TextureArray &other)
{
	id = other.id;
	width = other.width;
	height = other.height;
	nrChannels = other.nrChannels;
	depth = other.depth;
}
