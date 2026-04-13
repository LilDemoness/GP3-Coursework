#pragma once

#include <string>
#include <GL\glew.h>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include "stb_image.h"


class Texture
{
public:
	Texture(const std::string& file_path);
	Texture(const GLuint& texture_id);
	~Texture();

	void bind(unsigned int unit); // bind upto 32 textures

	inline const GLuint get_texture_id() const { return texture_id_; }

protected:
private:
	GLuint texture_id_;
};