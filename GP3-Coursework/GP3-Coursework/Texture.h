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
	Texture(const unsigned int width, const unsigned int height, GLint format);
	~Texture();
	static void clear();

	static std::shared_ptr<Texture> create_texture(const std::string& file_path);
	static std::shared_ptr<Texture> create_empty_texture(const unsigned int width, const unsigned int height, GLint format = GL_RGBA);
	static bool try_load_texture(const std::string& file_path, std::shared_ptr<Texture>& loaded_texture);
	static void register_texture_instance(const std::string& file_path, const std::shared_ptr<Texture> texture);


	void bind(unsigned int unit); // bind upto 32 textures

	inline const GLuint get_texture_id() const { return texture_id_; }

protected:
private:
	void generate_texture(const unsigned int width, const unsigned int height, GLint format, unsigned char* image_data);

	GLuint texture_id_;

	static std::unordered_map<std::string, std::shared_ptr<Texture>> all_loaded_textures_;
};