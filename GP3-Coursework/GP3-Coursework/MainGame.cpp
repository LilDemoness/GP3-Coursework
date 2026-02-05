#pragma once
#include "MainGame.h"

#include <iostream>

MainGame::MainGame() :
    game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
    game_state_(GameState::kPlay),
    shader_("..\\res\\shader")
{
    std::vector<Vertex> vertices =
    {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),  // Bottom Left
        Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)),  // Top Middle
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),  // Bottom Right
    };
    std::vector<unsigned int> indices = { 0, 1, 2 };

    mesh_ = Mesh(vertices, indices);
}
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
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // Set clear color to black
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
    game_display_.ClearDisplay();

    shader_.Bind();
    
    mesh_.Draw(); // Currently not drawing the mesh.

    if (false) // Run to display debug triangle (Shows that the Shader isn't the problem)
    {
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glEnd();
    }

    //glEnableClientState(GL_COLOR_ARRAY);
    //glEnd();

    game_display_.SwapBuffers();
}
