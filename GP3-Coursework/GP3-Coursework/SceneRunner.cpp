#pragma once
#include "SceneRunner.h"


SceneRunner::SceneRunner() :
	game_display_("OpenGL Game", kWindowWidth, kWindowHeight),
	delta_time_(0.0f),
	last_frame_start_time_(0)
{
	fixed_time_step_ = 1.0f / get_refresh_rate();

	Scene::on_exit_requested.subscribe(std::bind(&SceneRunner::start_scene_change, this, std::placeholders::_1));

	InputManager::register_input_event(SDLK_0, std::bind(&SceneRunner::return_to_main_menu, this));
	ShaderManager::load_shader("ScreenDisplayShader", "..\\res\\Shaders\\ScreenDisplayShader");
	initialise_screen_vao();
	game_display_.set_clear_color(0.15f, 0.15f, 0.15f, 1.0f);
}
SceneRunner::~SceneRunner()
{
	ShaderManager::clear();
	InputManager::clear();
	DLLManager::clear();
	Mesh::clear();
	Texture::clear();

	dispose_screen_vao();

	Scene::on_exit_requested.unsubscribe(std::bind(&SceneRunner::start_scene_change, this, std::placeholders::_1));
}


void SceneRunner::run()
{
	start_scene_change(Scene::GameMode::kMainMenu);
	set_active_scene();

	last_frame_start_time_ = SDL_GetPerformanceCounter();	// Prevent ultra-high initial 'delta_time_' values.
	while (current_game_mode_ != Scene::kExitGame)
	{
		if (current_game_mode_ != desired_game_mode_)
		{
			SDL_Delay(100);
			set_active_scene();
		}
		if (active_scene_ == nullptr)
			break;


		// Cache the high resolution time at the start of the frame.
		Uint64 start = SDL_GetPerformanceCounter();
		calculate_delta_time(start);

		game_display_.set_draw_to_framebuffer();

		// Detect Input.
		InputManager::process_input();

		// Update the scene.
		if (active_scene_)
			active_scene_->update(delta_time_);

		// Draw the scene.
		game_display_.clear_display();
		if (active_scene_)
			active_scene_->draw(&game_display_);

#if DISPLAY_FRAMERATE
		// Limit and (optionally) display our framerate.
		Uint64 frame_end_time = SDL_GetPerformanceCounter();
		cap_framerate(start, frame_end_time);
		display_framerate(start, SDL_GetPerformanceCounter());

		// Display the scene.
		draw_to_screen_quad();
		game_display_.swap_buffers();
#else
		// Display the scene.
		draw_to_screen_quad();
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
	const float kDelayMS = (1.0f / (float)kMaxFramerate) * 1000.0f;
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


void SceneRunner::initialise_screen_vao()
{
	glGenVertexArrays(1, &empty_vao_);
}
void SceneRunner::dispose_screen_vao()
{
	glDeleteVertexArrays(1, &empty_vao_);
}
void SceneRunner::draw_to_screen_quad()
{
	game_display_.set_draw_to_screen();

	ShaderManager::get_shader("ScreenDisplayShader")->bind();
	ShaderManager::get_shader("ScreenDisplayShader")->set_int("screen_texture", 0);
	glBindVertexArray(empty_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}



void SceneRunner::start_scene_change(Scene::GameMode new_game_mode)
{
	desired_game_mode_ = new_game_mode;
}
void SceneRunner::set_active_scene()
{
	if (active_scene_ != nullptr)
	{
		// Exit the active scene.
		active_scene_->on_exit_fulfilled();

		// Dispose of the active scene.
		active_scene_.reset();
		active_scene_ = nullptr;
	}

	// Determine & create our new scene.
	switch (desired_game_mode_)
	{
	case Scene::GameMode::kMainMenu: active_scene_ = std::make_unique<MainMenuScene>(); break;
	case Scene::GameMode::kGameplay: active_scene_ = std::make_unique<GameplayScene>(); break;
	case Scene::GameMode::kGameOver: active_scene_ = std::make_unique<GameOverScene>(); break;
	}

	// Check for creation error.
	if (active_scene_ == nullptr)
	{
		std::cerr << "Failed to set active scene pointer for: " << std::to_string(desired_game_mode_) << std::endl;
		return;
	}

	// Enter our new scene.
	active_scene_->enter(&game_display_);
	current_game_mode_ = desired_game_mode_;
}
void SceneRunner::return_to_main_menu()
{
	Scene::GameMode new_game_mode = Scene::GameMode::kMainMenu;
	start_scene_change(new_game_mode);
}