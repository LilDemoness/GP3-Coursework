#pragma once
#include <SDL/SDL.h>
#include <string>

class Window
{
public:
    Window(const std::string& title, int width, int height, Uint32 flags);
    ~Window();

    SDL_Window* getSDLWindow() const { return sdl_window_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    SDL_Window* sdl_window_;
    int width_;
    int height_;
};
