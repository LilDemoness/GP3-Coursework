#include "MainGame.h"
#include <iostream>

MainGame::MainGame() :
    game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
    game_state_(GameState::kPlay) 
{}
MainGame::~MainGame() 
{}


void MainGame::Run()
{
    InitSystems();
    GameLoop();
}


void MainGame::InitSystems()
{
    glEnable(GL_DEPTH_TEST); // Enable Z-buffering
    glEnable(GL_CULL_FACE);  // Enable face culling
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black
}

void MainGame::GameLoop()
{
    while (game_state_ != GameState::kExit)
    {
        ProcessInput();
        DrawGame();
    }
}

void MainGame::ProcessInput()
{
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt))
    {
        switch (evnt.type)
        {
        case SDL_QUIT:
            game_state_ = GameState::kExit;
            break;
        }
    }
}

void MainGame::DrawGame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    game_display_.SwapBuffers(); // Swap buffers via DisplayFacade
}
