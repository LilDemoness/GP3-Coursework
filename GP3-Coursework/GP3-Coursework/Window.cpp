#include "Window.h"
#include <stdexcept>

Window::Window(const std::string& title, const int width, const int height, const Uint32 flags)
    : sdl_window_(nullptr),
      width_(width),
      height_(height)
{
    sdl_window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!sdl_window_)
    {
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
}

Window::~Window()
{
    SDL_DestroyWindow(sdl_window_);
}
