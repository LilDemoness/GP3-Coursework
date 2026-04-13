#pragma once

#include "Event.h"
#include "DisplayFacade.h"

class Scene
{
public:
	enum GameMode
	{
		kMainMenu,
		kGameplay,
		kGameOver,
		kExitGame,
	};

	inline virtual GameMode get_game_mode() = 0;

	virtual void enter() = 0;
	virtual void update(float delta_time) = 0;
	virtual void draw(DisplayFacade* display_facade) = 0;
	virtual void on_exit_fulfilled() = 0;

	Event<GameMode> on_exit_requested;
};