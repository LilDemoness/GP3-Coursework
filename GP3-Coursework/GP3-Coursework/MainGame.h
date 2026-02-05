#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include "DisplayFacade.h" 

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:

	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();

	DisplayFacade _gameDisplay;
	GameState _gameState;

};

