# ifndef TEXTUREARRAY_HPP
# define TEXTUREARRAY_HPP

#include <iostream>

class TextureArray
{
    public:
        TextureArray();
        TextureArray(u_int id, int width, int height, int nrChannels, int depth);
        TextureArray(const TextureArray &other);

        ~TextureArray();

        unsigned int id;
        int width;
        int height;
        int nrChannels;
        int depth;
};



# endif