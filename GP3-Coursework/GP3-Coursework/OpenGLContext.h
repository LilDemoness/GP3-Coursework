#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>

class OpenGLContext
{
public:
    OpenGLContext(SDL_Window* window);
    ~OpenGLContext();

    void swap_buffers(SDL_Window* window);
    void set_clear_color(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a);
    void set_cull_face(const bool should_cull);

private:
    OpenGLContext() = delete;

    SDL_GLContext gl_context_;
};
