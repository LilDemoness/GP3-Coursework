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
#include "InputManager.h"

#include <iostream>
#include <string>
#include <memory>
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
	void load_physics_engine();


	void game_loop();
	void update_player();

	void process_input_events();
	void process_input();

	void draw_game();


	void calculate_delta_time(Uint64 frame_start_time);
	void cap_framerate(Uint64 frame_start_time);
	void display_framerate(Uint64 frame_start_time);
	float get_refresh_rate();


	DisplayFacade game_display_;
	GameState game_state_;
	GameObject object_1_;
	GameObject object_2_;
	std::shared_ptr<GameObject> player_;
	Camera camera_;
	Texture texture_;


	// Physics Function References.
	void (*add_thrust)(Transform*, float) = nullptr;
	void (*add_pitch)(Transform*, float) = nullptr;
	void (*add_yaw)(Transform*, float) = nullptr;
	void (*add_roll)(Transform*, float) = nullptr;

	void (*update_physics)(Transform*, float) = nullptr;


	float counter_;
	float delta_time_;
	Uint64 last_frame_start_time_;
	float fixed_time_step_ = 60;	// Default 60 physics updates per second.
};