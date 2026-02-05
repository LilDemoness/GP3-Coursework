#include "MainGame.h"
#include <iostream>

MainGame::MainGame()
    : _gameDisplay("OpenGL Game", 1024, 768), // Initialize the display wrapper
    _gameState(GameState::PLAY) {}

MainGame::~MainGame() {}

void MainGame::run() {
    initSystems();
    gameLoop();
}

void MainGame::initSystems() {
    glEnable(GL_DEPTH_TEST); // Enable Z-buffering
    glEnable(GL_CULL_FACE);  // Enable face culling
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black
}

void MainGame::gameLoop() {
    while (_gameState != GameState::EXIT) {
        processInput();
        drawGame();
    }
}

void MainGame::processInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
        case SDL_QUIT:
            _gameState = GameState::EXIT;
            break;
        }
    }
}

void MainGame::drawGame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers

    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    _gameDisplay.swapBuffers(); // Swap buffers via DisplayFacade
}
