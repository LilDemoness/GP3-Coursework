#pragma once
#include "Scene.h"

#include "InputManager.h"
#include "TextRenderer.h"

class GameOverScene : public Scene
{
public:
	GameOverScene();
	~GameOverScene();

	inline GameMode get_game_mode() override { return kGameOver; }


	void enter(DisplayFacade* display_facade) override;
	void update(float delta_time) override;
	void draw(DisplayFacade* display_facade) override;
	void on_exit_fulfilled() override;


private:
	void quit_game();
	void restart_game();
};