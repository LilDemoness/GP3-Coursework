#pragma once
#include "TextRenderer.h"


TextRenderer::TextRenderer()
{
    std::shared_ptr<Shader> font_shader = ShaderManager::get_instance().load_shader(FONT_SHADER_TAG, FONT_SHADER_PATH);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1280), 0.0f, static_cast<float>(720));
    font_shader->set_mat4("projection", projection);

    initialise_text();
    initialise_vertex_buffers();
}
TextRenderer::~TextRenderer()
{ }

TextRenderer* TextRenderer::get_instance()
{
    static TextRenderer* instance = new TextRenderer();
    return instance;
}


void TextRenderer::render_text(const std::string& text, float x, float y, float scale, glm::vec3 font_color, TextJustification justification)
{
    TextRenderer* text_renderer = get_instance();


    // Calculate the correct starting position for our given alignment.
    switch (justification)
    {
    case TextJustification::kLeftAligned: break;    // Left aligned starts at the passed 'x'.
    case TextJustification::kCentreAligned: x -= get_string_display_width(text, scale) / 2.0f; break;
    case TextJustification::kRightAligned: x -= get_string_display_width(text, scale); break;
    }


    // Change our depth function so that text always renders on the screen.
    glDepthFunc(GL_LEQUAL);

    // Bind our shader.
    std::shared_ptr<Shader> font_shader = ShaderManager::get_instance().get_shader(FONT_SHADER_TAG);
    font_shader->bind();
    font_shader->set_vec3("text_color", font_color);

    // Bind our texture.
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_renderer->vertex_array_object_);

    // Iterate through all characters in the text to render.
    for (auto c = text.begin(); c != text.end(); c++)
    {
        Character char_data = text_renderer->all_characters_[*c];

        float xpos = x + char_data.bearing.x * scale;
        float ypos = y - (char_data.size.y - char_data.bearing.y) * scale;

        float w = char_data.size.x * scale;
        float h = char_data.size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // Render glyph texture over our quad.
        glBindTexture(GL_TEXTURE_2D, char_data.texture_id);

        // Update content of VBO memory.
        glBindBuffer(GL_ARRAY_BUFFER, text_renderer->vertex_buffer_object_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad.
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance cursors for the next glyph (Advance is multiple of 1/64 pixels)
        x += (char_data.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }

    // Unbind values.
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Revert depth func.
    glDepthFunc(GL_LESS);
}
float TextRenderer::get_string_display_width(const std::string& text, const float scale)
{
    TextRenderer* text_renderer = get_instance();

    float total_width = 0.0f;
    for (auto c = text.begin(); c != text.end(); c++)
        total_width += (text_renderer->all_characters_[*c].advance >> 6) * scale;

    return total_width;
}


int TextRenderer::initialise_text()
{
    // Load the FreeType library.
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // Find the path to the font
    std::string font_name = FONT_PATH;


    // Load font as a FontFace.
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else
    {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // Disable byte-alignment restriction.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load the first 128 characters of ASCII set (Can expand later).
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load the character glyph.
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            // Generate the texture.
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // Set texture params.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Cache the character glyph.
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            all_characters_.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Destroy FreeType now we're finished.
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 0;
}
void TextRenderer::initialise_vertex_buffers()
{
    glGenVertexArrays(1, &vertex_array_object_);
    glGenBuffers(1, &vertex_buffer_object_);

    glBindVertexArray(vertex_array_object_);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}