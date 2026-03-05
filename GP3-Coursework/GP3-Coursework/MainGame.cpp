#pragma once

#include "MainGame.h"


MainGame::MainGame()
	: game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
	  game_state_(GameState::kPlay),
	  counter_(0.0f),
	  delta_time_(0.0f),
	  last_frame_start_time_(0.0f),

	  camera_(glm::vec3(0, 0, -5), 70.0f, (float)game_display_.get_width() / game_display_.get_height(), 0.01f, 1000.0f),
	  texture_("..\\res\\bricks.jpg"),
      object_1_(Mesh::create_triangle_mesh()),
      object_2_("..\\res\\monkey3.obj", glm::vec3(2.0f, 0.0f, 0.0f))
{
	ShaderManager::get_instance().load_shader("DefaultShader", "..\\res\\shader");
}
MainGame::~MainGame()
{
	ShaderManager::get_instance().clear();
}

void MainGame::Run()
{
	init_systems(); 
	game_loop();
}

void MainGame::init_systems()
{
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering.
	glEnable(GL_CULL_FACE);  // Enable face culling.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black.

	load_dlls();
	init_UBOs();
}
void MainGame::init_UBOs()
{
	// Create our UBOs.
	UBOManager::get_instance().create_ubo(kMatricesTag, sizeof(glm::mat4) * 3, 0);

	// Populate with initial data to ensure they aren't undefined.
	const glm::mat4 kIdentity = glm::mat4(1.0f);
	const size_t kMat4Size = sizeof(glm::mat4);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, 0, glm::value_ptr(kIdentity), kMat4Size);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, kMat4Size, glm::value_ptr(kIdentity), kMat4Size);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, kMat4Size * 2, glm::value_ptr(kIdentity), kMat4Size);

	// bind UBO to shaders.
	ShaderManager::get_instance().bind_all_shaders(kMatricesTag);
}

void MainGame::load_dlls()
{
	load_physics_engine_unsafe();
}
void MainGame::load_physics_engine_unsafe()
{
	DLLManager::get_instance().load_dll(PHYSICS_ENGINE_DLL_NAME);
	HelloWorldFunc hello_world = DLLManager::get_instance().get_function<HelloWorldFunc>(PHYSICS_ENGINE_DLL_NAME, "HelloWorld");

	hello_world();
}



void MainGame::game_loop()
{
	while (game_state_ != GameState::kExit)
	{
		// Cache the high resolution time at the start of the frame.
		Uint64 start = SDL_GetPerformanceCounter();
		calculate_delta_time(start);


		// Handle the frame.
		process_input();
		draw_game();


		// Limit and (optionally) display our framerate.
		counter_ += delta_time_;
		cap_framerate(start);
		//DisplayFramerate(start);
	}
}

void MainGame::process_input()
{
	// Get and process events
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				game_state_ = GameState::kExit;
				break;
		}
	}
	
}


void MainGame::draw_game()
{
	game_display_.clear_display();


	object_1_.get_transform()->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f * delta_time_));
	object_2_.get_transform()->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f * delta_time_));

	//std::shared_ptr<Shader> override_shader = ShaderManager::get_instance().get_shader("OverrideShader");
	texture_.bind(0);

	object_1_.draw(camera_);
	object_2_.draw(camera_);
				
	//glEnableClientState(GL_COLOR_ARRAY); 
	//glEnd();

	game_display_.swap_buffers();
} 



void MainGame::calculate_delta_time(Uint64 frame_start_time)
{
	delta_time_ = (frame_start_time - last_frame_start_time_) / (float)SDL_GetPerformanceFrequency();
	last_frame_start_time_ = frame_start_time;
}
void MainGame::cap_framerate(Uint64 frame_start_time)
{
	// Cache the high resolution time after handling our frame.
	Uint64 frame_end_time = SDL_GetPerformanceCounter();
	float elapsed_ms = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

	// Cap our framerate.
	// Note: Can be innaccurate by a small amount for high framerates (E.g. 120FPS).
	const float kDelayMS = (1.0f / (float)MAX_FRAMERATE) * 1000.0f;
	float duration = std::floor(kDelayMS - elapsed_ms);
	if (duration > 0)
	{
		SDL_Delay(duration);
	}
}
void MainGame::display_framerate(Uint64 frame_start_time)
{
	// Output our Framerate.
	Uint64 frame_end_time = SDL_GetPerformanceCounter();
	float elapsed_s = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency();
	std::cout << "Current FPS: " << std::to_string(std::round(1.0 / elapsed_s)) << ". Delta Time: " << std::to_string(delta_time_) << std::endl;
}