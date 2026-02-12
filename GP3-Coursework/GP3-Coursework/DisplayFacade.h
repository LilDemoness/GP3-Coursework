#pragma once

#include <string>
#include "Window.h"
#include "OpenGLContext.h"

class DisplayFacade
{
public:
    DisplayFacade(const std::string& title, const int width, const int height);
    ~DisplayFacade();

    void ClearDisplay();
    void SwapBuffers();

    void Resize(int newWidth, int newHeight);


    int get_width() const;
    int get_height() const;

private:
    DisplayFacade() = delete;

    Window* window_;            // Window wrapper
    OpenGLContext* context_;    // OpenGL context wrapper

    int screen_width_;          // Current width of the screen
    int screen_height_;         // Current height of the screen
};
