#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "ShaderManager.h"
#include "UBOManager.h"
#include "DLLManager.h"
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

#define PHYSICS_ENGINE_DLL_NAME "PhysicsEngine.dll"
using HelloWorldFunc = void(*)();

enum class GameState{kPlay, kExit};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void init_systems();
	void init_UBOs();

	void load_dlls();
	void load_physics_engine_unsafe();


	void process_input();
	void game_loop();
	void draw_game();

	void calculate_delta_time(Uint64 frame_start_time);
	void cap_framerate(Uint64 frame_start_time);
	void display_framerate(Uint64 frame_start_time);


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