#pragma once

#include <string>
#include <GL\glew.h>
#include <cassert>
#include <iostream>
#include "stb_image.h"

class Texture
{
public:
	Texture(const std::string& file_name);

	void Bind(unsigned int unit); // bind upto 32 textures

	~Texture();

protected:
private:

	GLuint texture_handler_;
};

