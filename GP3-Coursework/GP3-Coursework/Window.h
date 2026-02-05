#pragma once
#include <SDL/SDL.h>
#include <string>

class Window {
public:
    Window(const std::string& title, int width, int height, Uint32 flags);
    ~Window();

    SDL_Window* getSDLWindow() const { return sdlWindow; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    SDL_Window* sdlWindow;
    int width;
    int height;
};
