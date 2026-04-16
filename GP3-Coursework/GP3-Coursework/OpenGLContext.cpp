#include "OpenGLContext.h"
#include <stdexcept>

OpenGLContext::OpenGLContext(SDL_Window* window)
{
    gl_context_ = SDL_GL_CreateContext(window);
    if (!gl_context_)
    {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        throw std::runtime_error("Failed to initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(error))));
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

OpenGLContext::~OpenGLContext()
{
    SDL_GL_DeleteContext(gl_context_);
}

void OpenGLContext::set_clear_color(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) { glClearColor(r, g, b, a); }
void OpenGLContext::swap_buffers(SDL_Window* window_)
{
    SDL_GL_SwapWindow(window_);
}
void OpenGLContext::set_cull_face(const bool should_cull)
{
    if (should_cull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}