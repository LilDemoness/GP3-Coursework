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
	static void clear();

	static std::shared_ptr<Texture> create_texture(const std::string& file_path);
	static bool try_load_texture(const std::string& file_path, std::shared_ptr<Texture>& loaded_texture);
	static void register_texture_instance(const std::string& file_path, const std::shared_ptr<Texture> texture);


	void bind(unsigned int unit); // bind upto 32 textures

	inline const GLuint get_texture_id() const { return texture_id_; }

protected:
private:

	GLuint texture_id_;

	static std::unordered_map<std::string, std::shared_ptr<Texture>> all_loaded_textures_;
};