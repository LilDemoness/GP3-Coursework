#include "Window.h"
#include <iostream>

Window::Window(const std::string& title, int width, int height, Uint32 flags) :
    width(width),
    height(height)
{
    // Ensure SDL is initialized before creating the window
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL failed to initialize");
    }

    // Set OpenGL attributes before creating the window
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    sdl_window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    if (!sdl_window_)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit(); // Clean up before throwing.
        throw std::runtime_error("Window creation failed");
    }
}

Window::~Window()
{
    if (sdl_window_)
        SDL_DestroyWindow(sdl_window_);
    
    SDL_Quit(); // Cleanup SDL when the window is destroyed.
}
