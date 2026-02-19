#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "ShaderManager.h"
#include "UBOManager.h"
#include "GameObject.h"
#include "Texture.h"
#include "Transform.h"
#include "DisplayFacade.h"
#include "Camera.h"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>


#define MAX_FRAMERATE 60
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
	void InitUBOs();

	void ProcessInput();
	void GameLoop();
	void DrawGame();

	void CalculateDeltaTime(Uint64 frame_start_time);
	void CapFramerate(Uint64 frame_start_time);
	void DisplayFramerate(Uint64 frame_start_time);


	DisplayFacade game_display_;
	GameState game_state_;
	GameObject object_1_;
	GameObject object_2_;
	Camera camera_;
	Texture texture_;


	float counter_;
	float delta_time_;
	Uint64 last_frame_start_time_;
};