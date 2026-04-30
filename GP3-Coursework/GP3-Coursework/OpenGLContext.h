#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>

#include "ShaderManager.h"

class OpenGLContext
{
public:
    OpenGLContext(SDL_Window* window);
    ~OpenGLContext();

    void swap_buffers(SDL_Window* window);
    void set_clear_color(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a);
    void set_cull_face(const bool should_cull);

    void set_draw_to_framebuffer();
    void draw_to_screen();
    void copy_screen_texture(GLuint texture_id);

private:
    OpenGLContext() = delete;
    void generate_screen_texture(SDL_Window* window);

    SDL_GLContext gl_context_;

    unsigned int framebuffer_;
    unsigned int texture_colorbuffer_;
    unsigned int render_buffer_object_;

    unsigned int empty_vao_;
};
