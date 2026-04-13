#pragma once

#include "Texture.h"

#include <GL\glew.h>
#include <iostream>
#include <vector>
#include <memory>

#define CUBE_FACE_COUNT 6

class Cubemap
{
public:
	static std::unique_ptr<Texture> create_cubemap_texture(const std::string& file_path_no_extension, const std::string& file_type);
	static std::unique_ptr<Texture> create_cubemap_texture_from_single(const std::string& file_path_no_extension, const std::string& file_type);

private:
	static std::string get_face_name(const std::string file_name, const unsigned int face_index, const std::string& file_type);
	static inline void get_offsets_for_index(const int index, int& x_offset_multiplier, int& y_offset_multiplier);
};