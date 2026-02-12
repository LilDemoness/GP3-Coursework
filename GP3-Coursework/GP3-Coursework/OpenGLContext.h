#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>

class OpenGLContext
{
public:
    OpenGLContext(SDL_Window* window);
    ~OpenGLContext();

    void swapBuffers(SDL_Window* window);

private:
    OpenGLContext() = delete;

    SDL_GLContext gl_context_;
};
