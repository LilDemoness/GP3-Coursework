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

    generate_screen_texture(window);

    glGenVertexArrays(1, &empty_vao_);
    ShaderManager::load_shader("ScreenDisplayShader", "..\\res\\Shaders\\ScreenDisplayShader");
}
void OpenGLContext::generate_screen_texture(SDL_Window* window)
{
    // Cache required values.
    int screen_width = 0, screen_height = 0;
    SDL_GL_GetDrawableSize(window, &screen_width, &screen_height);


    // Generate the Framebuffer.
    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    // Generate the Display Texture.
    glGenTextures(1, &texture_colorbuffer_);
    glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach our display texture to the generated framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer_, 0);


    // Create a RenderBufferObject to allow for Depth and Stencil testing.
    glGenRenderbuffers(1, &render_buffer_object_);
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach our renderbuffer to the framebuffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_);


    // Check for errors.
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR: Framebuffer is not complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLContext::~OpenGLContext()
{
    glDeleteRenderbuffers(1, &render_buffer_object_);
    glDeleteFramebuffers(1, &framebuffer_);

    glDeleteVertexArrays(1, &empty_vao_);
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


void OpenGLContext::set_draw_to_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}
void OpenGLContext::draw_to_screen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_);

    // Draw to the screen texture.
    ShaderManager::get_shader("ScreenDisplayShader")->bind();
    ShaderManager::get_shader("ScreenDisplayShader")->set_int("screen_texture", 0);
    glBindVertexArray(empty_vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
void OpenGLContext::copy_screen_texture(GLuint texture_id)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    glDisable(GL_DEPTH_TEST);

    // Draw to the texture.
    ShaderManager::get_shader("ScreenDisplayShader")->bind();
    ShaderManager::get_shader("ScreenDisplayShader")->set_int("screen_texture", 0);
    glBindVertexArray(empty_vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Reset bindings.
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer_, 0);

    glEnable(GL_DEPTH_TEST);
}