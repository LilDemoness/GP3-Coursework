#pragma once

#include <string>
#include "Window.h"
#include "OpenGLContext.h"

class DisplayFacade
{
public:
    DisplayFacade(const std::string& title, const int width, const int height);
    ~DisplayFacade();

    void clear_display();
    void swap_buffers();

    void resize(int newWidth, int newHeight);

    inline void set_clear_color(const GLclampf r, const GLclampf g, const GLclampf b, const GLclampf a) { context_->set_clear_color(r, g, b, a); }
    inline void set_cull_backface(const bool should_cull) { context_->set_cull_face(should_cull); }

    inline void set_draw_to_framebuffer() { context_->set_draw_to_framebuffer(); }
    inline void draw_to_screen() { context_->draw_to_screen(); }
    inline void copy_screen_texture(GLuint texture_id) { context_->copy_screen_texture(texture_id); }

    inline int get_width() const { return screen_width_; }
    inline int get_height() const { return screen_height_; }

private:
    DisplayFacade() = delete;

    Window* window_;            // Window wrapper
    OpenGLContext* context_;    // OpenGL context wrapper

    int screen_width_;          // Current width of the screen
    int screen_height_;         // Current height of the screen
};
