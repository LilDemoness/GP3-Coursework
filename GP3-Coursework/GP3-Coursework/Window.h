#pragma once
#include <SDL/SDL.h>
#include <string>

class Window
{
public:
    Window(const std::string& title, const int width, const int height, const Uint32 flags);
    ~Window();

    SDL_Window* get_SDL_window() const { return sdl_window_; }
    int get_width() const   { return width_; }
    int get_height() const  { return height_; }

private:
    Window() = delete;

    SDL_Window* sdl_window_;
    int width_;
    int height_;
};
