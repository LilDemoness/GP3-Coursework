#include "DisplayFacade.h"


DisplayFacade::DisplayFacade()
    : window("Default Title", 800, 600, SDL_WINDOW_OPENGL), context(window.getSDLWindow()), screenWidth(800), screenHeight(800) {                     // Initialize OpenGL context
}

DisplayFacade::DisplayFacade(const std::string& title, const int width, const int height)
    : window(title, width, height, SDL_WINDOW_OPENGL),
    context(window.getSDLWindow()),
    screenWidth(width),
    screenHeight(height) {}

DisplayFacade::~DisplayFacade() {}

void DisplayFacade::resize(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
    SDL_SetWindowSize(window.getSDLWindow(), newWidth, newHeight);
}

void DisplayFacade::swapBuffers() {
    context.swapBuffers(window.getSDLWindow());
}

int DisplayFacade::getWidth() const {
    return screenWidth;
}

int DisplayFacade::getHeight() const {
    return screenHeight;
}
