# ifndef TEXTUREARRAY_HPP
# define TEXTUREARRAY_HPP

#include "Texture.hpp"
#include <iostream>

class TextureArray : private Texture
{
    public:
        TextureArray();
        TextureArray(u_int id, int width, int height, int nrChannels, int depth);
        TextureArray(const TextureArray &other);

        ~TextureArray();

        int depth;
};



# endif