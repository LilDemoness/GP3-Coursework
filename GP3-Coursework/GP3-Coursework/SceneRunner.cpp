#pragma once
#include "SceneRunner.h"


SceneRunner::SceneRunner() :
	game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
	delta_time_(0.0f),
	last_frame_start_time_(0.0f)
{
	fixed_time_step_ = 1.0f / get_refresh_rate();


	// Can Remove?
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering.
	glEnable(GL_CULL_FACE);  // Enable face culling.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black.
}
SceneRunner::~SceneRunner()
{
	delete active_scene_;
}


void SceneRunner::run()
{
	set_active_scene(Scene::GameMode::kMainMenu);

	last_frame_start_time_ = SDL_GetPerformanceCounter();	// Prevent ultra-high initial 'delta_time_' values.
	while (current_game_mode_ != Scene::kExitGame)
	{
		// Cache the high resolution time at the start of the frame.
		Uint64 start = SDL_GetPerformanceCounter();
		calculate_delta_time(start);

		// Detect Input.
		InputManager::get_instance().process_input();
		if (active_scene_ == nullptr)
		{
			std::cerr << "An error occured during a scene swap called from input." << std::endl;
			break;
		}

		// Update the scene.
		active_scene_->update(delta_time_);
		if (active_scene_ == nullptr)
		{
			std::cerr << "An error occured during a scene swap called in Update." << std::endl;
			break;
		}

		// Draw the scene.
		game_display_.clear_display();
		active_scene_->draw(&game_display_);

#if DISPLAY_FRAMERATE
		// Limit and (optionally) display our framerate.
		Uint64 frame_end_time = SDL_GetPerformanceCounter();
		cap_framerate(start, frame_end_time);
		display_framerate(start, SDL_GetPerformanceCounter());

		// Display the scene.
		game_display_.swap_buffers();
#else
		// Display the scene.
		game_display_.swap_buffers();

		// Limit and (optionally) display our framerate.
		Uint64 frame_end_time = SDL_GetPerformanceCounter();
		cap_framerate(start, frame_end_time);
#endif
	}
}


void SceneRunner::calculate_delta_time(Uint64 frame_start_time)
{
	delta_time_ = (frame_start_time - last_frame_start_time_) / (float)SDL_GetPerformanceFrequency();
	last_frame_start_time_ = frame_start_time;
}
void SceneRunner::cap_framerate(Uint64 frame_start_time, Uint64 frame_end_time)
{
	// Determine the duration of our frame.
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
void SceneRunner::display_framerate(Uint64 frame_start_time, Uint64 frame_end_time)
{
	// Output our Framerate.
	float elapsed_s = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency();
	TextRenderer::render_text("Current FPS: " + std::to_string((int)std::round(1.0 / elapsed_s)), 5.0f, game_display_.get_height() - 25.0f, 0.5f, glm::vec3(1.0f), TextRenderer::kLeftAligned);
	TextRenderer::render_text("Delta Time : " + std::to_string((int)(delta_time_ * 1000.0f)) + "ms", 5.0f, game_display_.get_height() - 55.0f, 0.5f, glm::vec3(1.0f), TextRenderer::kLeftAligned);
}


float SceneRunner::get_refresh_rate()
{
	SDL_DisplayMode display_mode;
	if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0 && display_mode.refresh_rate > 0)
	{
		return static_cast<float>(display_mode.refresh_rate);
	}

	const float kDefaultRefreshRate = 60.0f;
	std::cout << "Failed to retrieve monitor Refresh Rate. Defaulting to " << kDefaultRefreshRate << std::endl;
	return kDefaultRefreshRate;
}



void SceneRunner::set_active_scene(Scene::GameMode new_game_mode)
{
	if (active_scene_ != nullptr)
	{
		// Exit the active scene.
		active_scene_->on_exit_fulfilled();
		active_scene_->on_exit_requested.unsubscribe(std::bind(&SceneRunner::set_active_scene, this, std::placeholders::_1));

		// Dispose of the active scene.
		delete active_scene_;
		active_scene_ = nullptr;
	}

	// Determine & create our new scene.
	switch (new_game_mode)
	{
	case Scene::GameMode::kMainMenu: active_scene_ = new MainMenuScene(); break;
	}

	// Check for creation error.
	if (active_scene_ == nullptr)
	{
		std::cerr << "Failed to set active scene pointer for: " << std::to_string(new_game_mode) << std::endl;
		return;
	}

	// Enter our new scene.
	active_scene_->on_exit_requested.subscribe(std::bind(&SceneRunner::set_active_scene, this, std::placeholders::_1));
	active_scene_->enter();
}