#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

class Texture
{
public:
	Texture();
	Texture(u_int id, int width, int height, int nrChannels);
	Texture(const Texture &other);

	~Texture();

	unsigned int id;
	int width;
	int height;
	int nrChannels;
};

#endif