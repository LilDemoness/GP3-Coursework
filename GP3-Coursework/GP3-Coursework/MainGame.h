#pragma once
#include <SDL\SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include "Shader.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "DisplayFacade.h"
#include "Camera.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

enum class GameState{PLAY, EXIT};

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
	Mesh mesh_1_;
	Mesh mesh_2_;
	Camera my_camera_;
	Texture texture_;
	Transform transform_;

	float counter_;
};