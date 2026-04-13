#pragma once
#include "Cubemap.h"

std::unique_ptr<Texture> Cubemap::create_cubemap_texture(const std::string& file_path_no_extension, const std::string& file_type)
{
    // Load our Cubemap Faces.
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < CUBE_FACE_COUNT; i++)
    {
        std::string face_name = get_face_name(file_path_no_extension, i, file_type);
        unsigned char* data = stbi_load(face_name.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);

            std::cout << "Cubemap Texture Loaded from Path: " << face_name << std::endl;
        }
        else
        {
            std::cout << "Cubemap Texture Failed to Load from Path: " << face_name << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    // Create and return our Cubemap Texture.
    return std::make_unique<Texture>(textureID);
};
std::unique_ptr<Texture> Cubemap::create_cubemap_texture_from_single(const std::string& file_path_no_extension, const std::string& file_type)
{
    // Load our Cubemap Faces.
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);


    int width, height, nrChannels;
    std::string file_path = file_path_no_extension + file_type;
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        for (unsigned int i = 0; i < CUBE_FACE_COUNT; i++)
        {
            data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, 0);
            int offset_x = 512;
            int offset_y = 512;
            get_offsets_for_index(i, offset_x, offset_y);

            glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, offset_y);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, offset_x);


            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width / 4, height / 3, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );

            stbi_image_free(data);

            std::cout << get_face_name("", i, "") << ": " << offset_x << " | " << offset_y << std::endl;
        }

        //stbi_image_free(data);
    }
    else
    {
        std::cout << "Cubemap Texture Failed to Load from Path: " << file_path << std::endl;
        stbi_image_free(data);
    }


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);


    return std::make_unique<Texture>(textureID);
}

std::string Cubemap::get_face_name(const std::string file_name, const unsigned int face_index, const std::string& file_type)
{
    switch (face_index)
    {
    case 0: return file_name + "_left" + file_type;
    case 1: return file_name + "_right" + file_type;
    case 2: return file_name + "_top" + file_type;
    case 3: return file_name + "_bottom" + file_type;
    case 4: return file_name + "_front" + file_type;
    case 5: return file_name + "_back" + file_type;
    }
}
inline void Cubemap::get_offsets_for_index(const int index, int& x_offset_multiplier, int& y_offset_multiplier)
{
    // Note: Index order is based on OpenGL's layout for it's GL_TEXTURE_CUBE_MAP face values.
    switch (index)
    {
    case 0: x_offset_multiplier *= 2; y_offset_multiplier *= 1; break; // Right.    (2-1)
    case 1: x_offset_multiplier *= 0; y_offset_multiplier *= 1; break; // Left.     (0-1)
    case 2: x_offset_multiplier *= 1; y_offset_multiplier *= 0; break; // Top       (1-0).
    case 3: x_offset_multiplier *= 1; y_offset_multiplier *= 2; break; // Bottom.   (1-2)
    case 4: x_offset_multiplier *= 1; y_offset_multiplier *= 1; break; // Front.    (1-1)
    case 5: x_offset_multiplier *= 3; y_offset_multiplier *= 1; break; // Back.     (3-1)
    }
}