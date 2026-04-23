#pragma once
#include "Texture.h"


Texture::Texture(const std::string& file_path)
{
	int width, height, numComponents; //width, height, and no of components of image
	unsigned char* imageData = stbi_load((file_path).c_str(), &width, &height, &numComponents, 4); //load the image and store the data

	if (imageData == NULL)
	{
		std::cerr << "texture load failed" << file_path << std::endl;
	}

	glGenTextures(1, &texture_id_); // number of and address of textures
	glBindTexture(GL_TEXTURE_2D, texture_id_); //bind texture - define type 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture outside width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture outside height

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification (texture is smaller than area)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnifcation (texture is larger)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); //Target, Mipmapping Level, Pixel Format, Width, Height, Border Size, Input Format, Data Type of Texture, Image Data

	stbi_image_free(imageData);
}
Texture::Texture(const GLuint& texture_id) :
	texture_id_(texture_id)
{}
Texture::~Texture()
{
	glDeleteTextures(1, &texture_id_); // number of and address of textures
}

void Texture::clear()
{
	all_loaded_textures_.clear();
}


std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::all_loaded_textures_;
std::shared_ptr<Texture> Texture::create_texture(const std::string& file_path)
{
	auto it = all_loaded_textures_.find(file_path);
	if (it != all_loaded_textures_.end())
		return it->second;

	std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(file_path);
	all_loaded_textures_.emplace(file_path, new_texture);
	return new_texture;
}


void Texture::bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31); // check we are working with one of the 32 textures

	glActiveTexture(GL_TEXTURE0 + unit); //set acitve texture unit
	glBindTexture(GL_TEXTURE_2D, texture_id_); //type of and texture to bind to unit
}
