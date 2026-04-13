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

	virtual ~Scene();

	inline virtual GameMode get_game_mode() = 0;

	virtual void enter(DisplayFacade* display_facade) = 0;
	virtual void update(float delta_time) = 0;
	virtual void draw(DisplayFacade* display_facade) = 0;
	virtual void on_exit_fulfilled() = 0;

	static int score_;
	static Event<GameMode> on_exit_requested;
};