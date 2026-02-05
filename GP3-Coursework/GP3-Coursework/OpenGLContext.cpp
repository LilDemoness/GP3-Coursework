#pragma once
#include "OpenGLContext.h"

#include <stdexcept>

OpenGLContext::OpenGLContext(SDL_Window* window)
{
    gl_context_ = SDL_GL_CreateContext(window);
    if (!gl_context_)
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    

    GLenum error = glewInit();
    if (error != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(error))));
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
OpenGLContext::~OpenGLContext() 
{
    SDL_GL_DeleteContext(gl_context_);
}


void OpenGLContext::SwapBuffers(SDL_Window* window)
{
    SDL_GL_SwapWindow(window);
}
