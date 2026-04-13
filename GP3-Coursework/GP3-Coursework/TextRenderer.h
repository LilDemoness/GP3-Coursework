#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ShaderManager.h"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <map>
#include <string>
#include <iostream>
#include <filesystem>

#define FONT_SHADER_TAG "FontShader"
#define FONT_SHADER_PATH "..\\res\\FontShader"

#define FONT_PATH "..\\res\\Fonts\\vga850.ttf"

class TextRenderer
{
public:
	// Note: Text should be rendered after everything that is drawn behind it, otherwise transparency becomes funky and you can sometimes see the original/uninitialised background through the edges of the glyphs.
	static void render_text(std::string text, float x, float y, float scale, glm::vec3 font_color);


private:
	TextRenderer();
	~TextRenderer();

	// Delete Copy Constructor.
	TextRenderer(const TextRenderer& other) = delete;
	TextRenderer& operator= (const TextRenderer& other) = delete;

	static TextRenderer* get_instance();

	struct Character
	{
		unsigned int texture_id;	// ID handle of the glyph texture.
		glm::ivec2 size;			// Size of the glyph.
		glm::ivec2 bearing;			// Offset from baseline to left/top of the glyph.
		unsigned int advance;		// Offset to the next glyph.
	};

	int initialise_text();
	std::map<char, Character> all_characters_;

	void initialise_vertex_buffers();
	GLuint vertex_array_object_;
	GLuint vertex_buffer_object_;
};