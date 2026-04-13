#pragma once

#include "Scene.h"
#include "MainMenuScene.h"

#include "DisplayFacade.h"
#include "TextRenderer.h"

#define MAX_FRAMERATE 60
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define DISPLAY_FRAMERATE true

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
	float fixed_time_step_ = 60;	// Default 60 physics updates per second.


	// Scenes.
	Scene::GameMode current_game_mode_;
	Scene* active_scene_;

	void set_active_scene(Scene::GameMode new_game_mode);
};