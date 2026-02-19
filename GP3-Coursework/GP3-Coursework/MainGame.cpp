#pragma once

#include "MainGame.h"


MainGame::MainGame()
	: game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
	  game_state_(GameState::PLAY),
	  counter_(0.0f),
	  delta_time_(0.0f),
	  last_frame_start_time_(0.0f),

	  camera_(glm::vec3(0, 0, -5), 70.0f, (float)game_display_.get_width() / game_display_.get_height(), 0.01f, 1000.0f),
	  texture_("..\\res\\bricks.jpg"),
      object_1_(Mesh::CreateTriangleMesh()),
      object_2_("..\\res\\monkey3.obj", glm::vec3(2.0f, 0.0f, 0.0f))
{
	ShaderManager::get_instance().LoadShader("DefaultShader", "..\\res\\shader");
}
MainGame::~MainGame()
{
	ShaderManager::get_instance().Clear();
}

void MainGame::Run()
{
	InitSystems(); 
	GameLoop();
}

void MainGame::InitSystems()
{
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering.
	glEnable(GL_CULL_FACE);  // Enable face culling.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black.
}

void MainGame::GameLoop()
{
	while (game_state_ != GameState::EXIT)
	{
		// Cache the high resolution time at the start of the frame.
		Uint64 start = SDL_GetPerformanceCounter();
		CalculateDeltaTime(start);


		// Handle the frame.
		ProcessInput();
		DrawGame();


		// Limit and (optionally) display our framerate.
		counter_ += delta_time_;
		CapFramerate(start);
		//DisplayFramerate(start);
	}
}

void MainGame::ProcessInput()
{
	// Get and process events
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				game_state_ = GameState::EXIT;
				break;
		}
	}
	
}


void MainGame::DrawGame()
{
	game_display_.ClearDisplay();


	//object_1_.get_transform()->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f * delta_time_));
	//object_2_.get_transform()->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f * delta_time_));

	//std::shared_ptr<Shader> override_shader = ShaderManager::get_instance().GetShader("OverrideShader");
	texture_.Bind(0);

	object_1_.Draw(camera_);
	object_2_.Draw(camera_);
				
	//glEnableClientState(GL_COLOR_ARRAY); 
	//glEnd();

	game_display_.SwapBuffers();
} 



void MainGame::CalculateDeltaTime(Uint64 frame_start_time)
{
	delta_time_ = (frame_start_time - last_frame_start_time_) / (float)SDL_GetPerformanceFrequency();
	last_frame_start_time_ = frame_start_time;
}
void MainGame::CapFramerate(Uint64 frame_start_time)
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
void MainGame::DisplayFramerate(Uint64 frame_start_time)
{
	// Output our Framerate.
	Uint64 frame_end_time = SDL_GetPerformanceCounter();
	float elapsed_s = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency();
	std::cout << "Current FPS: " << std::to_string(std::round(1.0 / elapsed_s)) << ". Delta Time: " << std::to_string(delta_time_) << std::endl;
}