#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include "DisplayFacade.h"
#include "Shader.h"
#include "Mesh.h"


#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768


enum class GameState{kPlay, kExit};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void InitSystems();
	void ProcessInput();
	void GameLoop();
	void DrawGame();

	DisplayFacade game_display_;
	GameState game_state_;

	Shader shader_;
	Mesh mesh_;
};

