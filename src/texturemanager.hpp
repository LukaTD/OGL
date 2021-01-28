#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>
#include <iterator>
#include <algorithm>

#include <GLEW/glew.h>

#include "types.hpp"

struct TextureInfo
{
	std::string name;			// name used to reference the created texture
	std::string pathToImage; 	// path to file containing texture data
};

struct CubemapTextureInfo
{
	std::string 			  name;				// name used to reference the created cubemap texture
	std::array<std::string,6> pathsToImages; 	// paths to the textures containg the data for each of the 6 faces of the cubemap texture (order matters)
};

struct TextureManager
{
    std::unordered_map<std::string,u32> textures;

	~TextureManager();

    void CreateTextureFromImage(const TextureInfo &textureInfo);
	void CreateCubemapFromImages(const CubemapTextureInfo &cubemapInfo);

	void DeleteTexture(const std::string &textureName);
	void DeleteSelectedTextures(const std::vector<std::string> &textureNames);
	void DeleteAllTextures();
};
