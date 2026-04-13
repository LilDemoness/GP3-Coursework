#pragma once
#include "Scene.h"

#include "InputManager.h"
#include "TextRenderer.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene();
	~MainMenuScene();

	inline GameMode get_game_mode() override { return kMainMenu; }


	void enter() override;
	void update(float delta_time) override;
	void draw(DisplayFacade* display_facade) override;
	void on_exit_fulfilled() override;

private:
	void start_game();
};