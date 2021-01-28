#include "texturemanager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureManager::~TextureManager()
{
	if(!this->textures.empty())
	{
		std::cout << "Not all textures have been manually deleted. " 
					 "Automatically deleting:" << std::endl;
		for(const auto &[textureName,texture] : this->textures)
			std::cout << '\t' << textureName << std::endl;
		
		DeleteAllTextures();
	}
}

void TextureManager::CreateTextureFromImage(const TextureInfo &textureInfo)
{
	stbi_set_flip_vertically_on_load(true);

    u32 textureID;

    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D,textureID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    i32 width,height,numChannels;

    u8 *imageData = stbi_load(textureInfo.pathToImage.c_str(),&width,&height,&numChannels,0);

    if(!imageData)
    {
        std::cout << "Image data could not be loaded from \"" << textureInfo.pathToImage << "\"!" << std::endl;
        exit(-1);
    }
    
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imageData);

    this->textures.insert(std::make_pair(textureInfo.name,textureID));  
}

// pathsToImages names must be in the right order (+X,-X,+Y,-Y,+Z,-Z)
void TextureManager::CreateCubemapFromImages(const CubemapTextureInfo &cubemapInfo)
{
	stbi_set_flip_vertically_on_load(false);

	u32 cubemapID;

	glGenTextures(1,&cubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubemapID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	i32 width,height,numChannels;

	for(u32 i = 0;i < cubemapInfo.pathsToImages.size();i++)
	{
		u8 *imageData = stbi_load(cubemapInfo.pathsToImages[i].c_str(),&width,&height,&numChannels,0);

		if(!imageData)
		{
			std::cout << "Image data could not be loaded from \"" << cubemapInfo.pathsToImages[i] << "\"!" 
					  << std::endl;
			exit(-1);
		}
		
		// texure orientation macros are defined in order 
		// from 0x8515 to 0x851A (+X,-X,+Y,-Y,+Z,-Z)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,imageData);

    	stbi_image_free(imageData); 
	}

	this->textures.insert(std::make_pair(cubemapInfo.name,cubemapID));
}

void TextureManager::DeleteTexture(const std::string &textureName)
{
	const u32 textureID = this->textures[textureName];
	this->textures.erase(textureName);

	glDeleteTextures(1,&textureID);
}

void TextureManager::DeleteSelectedTextures(const std::vector<std::string> &textureNames)
{
	const u32 numberOfTextures = textureNames.size();

	std::vector<u32> sequentialTextures;
	sequentialTextures.resize(numberOfTextures);

	for(u32 i = 0;i < numberOfTextures;i++)
	{
		sequentialTextures[i] = this->textures[textureNames[i]];
		this->textures.erase(textureNames[i]);
	}

	glDeleteTextures(numberOfTextures,sequentialTextures.data());
}

void TextureManager::DeleteAllTextures()
{
	const u32 numberOfTextures = this->textures.size();

	std::vector<u32> sequentialTextures;
	sequentialTextures.resize(numberOfTextures);

	u32 i = 0;
	for(const auto &[textureName,texture] : this->textures)
	{
		sequentialTextures[i] = texture;
		i++;
	}
	this->textures.clear();

	glDeleteTextures(numberOfTextures,sequentialTextures.data());
}