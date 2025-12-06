#ifndef TEXTURECUBEMAP_HPP
#define TEXTURECUBEMAP_HPP

#include "Texture.hpp"
#include <iostream>

class TextureCubeMap : private Texture
{
public:
    TextureCubeMap();
    TextureCubeMap(u_int id);
    TextureCubeMap(const TextureCubeMap &other);

    ~TextureCubeMap();
};

#endif