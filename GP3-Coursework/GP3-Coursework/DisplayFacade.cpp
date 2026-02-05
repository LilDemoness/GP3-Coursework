#pragma once
#include "DisplayFacade.h"


DisplayFacade::DisplayFacade(const std::string& title, const int width, const int height) :
    window_(new Window(title, width, height, SDL_WINDOW_OPENGL)),
    context_(new OpenGLContext(window_->getSDLWindow())),
    screen_width_(width),
    screen_height_(height)
{}

DisplayFacade::~DisplayFacade()
{
    // Ensure that we dispose of the Context before the Window (Reverse order from creation)
    delete context_;
    delete window_;
}


void DisplayFacade::ClearDisplay()
{
    // Note: Move to context?
    // Clear colour and depth buffer - set colour to colour defined in glClearColor
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void DisplayFacade::SwapBuffers()
{
    context_->SwapBuffers(window_->getSDLWindow());
}


void DisplayFacade::Resize(int newWidth, int newHeight)
{
    screen_width_ = newWidth;
    screen_height_ = newHeight;
    SDL_SetWindowSize(window_->getSDLWindow(), newWidth, newHeight);
}


int DisplayFacade::get_width() const 
{
    return screen_width_;
}
int DisplayFacade::get_height() const
{
    return screen_height_;
}
