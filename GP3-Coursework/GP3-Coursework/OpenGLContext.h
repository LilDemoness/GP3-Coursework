#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

class OpenGLContext
{
public:
    OpenGLContext(SDL_Window* window);
    ~OpenGLContext();

    void SwapBuffers(SDL_Window* window);

private:
    SDL_GLContext gl_context_;
};
