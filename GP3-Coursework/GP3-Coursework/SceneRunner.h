#pragma once

#include "Scene.h"
#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "GameOverScene.h"

#include "DisplayFacade.h"
#include "TextRenderer.h"
#include "InputManager.h"

constexpr int kMaxFramerate = 60;
constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

#define DISPLAY_FRAMERATE false

class SceneRunner
{
public:
	SceneRunner();
	~SceneRunner();

	void run();

private:
	// Display.
	void calculate_delta_time(Uint64 frame_start_time);
	void cap_framerate(Uint64 frame_start_time, Uint64 frame_end_time);
	void display_framerate(Uint64 frame_start_time, Uint64 frame_end_time);
	float get_refresh_rate();

	DisplayFacade game_display_;

	float delta_time_;
	Uint64 last_frame_start_time_;
	float fixed_time_step_ = 60.0f;	// Default 60 physics updates per second.


	// Scenes.
	Scene::GameMode current_game_mode_;
	Scene::GameMode desired_game_mode_;
	std::unique_ptr<Scene> active_scene_;

	void start_scene_change(Scene::GameMode new_game_mode);
	void set_active_scene();
	void return_to_main_menu();


	void quit_game();
};