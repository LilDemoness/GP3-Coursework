#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "ShaderManager.h"
#include "UBOManager.h"
#include "DLLManager.h"
#include "GameObject.h"
#include "Texture.h"
#include "Transform.h"
#include "DisplayFacade.h"
#include "Camera.h"
#include "InputManager.h"
#include "Collider.h"
#include "ObjectPool.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Skybox.h"
#include "WorldBorderVisuals.h"
#include "TextRenderer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <cmath>
#include <set>


#define MAX_FRAMERATE 60
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PLAY_SPACE_RADIUS 25.0f

#define PHYSICS_ENGINE_DLL_NAME "PhysicsEngine.dll"
using HelloWorldFunc = void(*)();

enum class GameState{kPlay, kExit};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void Run();

private:
	void init_systems();
	void init_UBOs();

	void load_dlls();
	void load_physics_engine();


	void game_loop();
	void update_player();
	void handle_collisions();

	void handle_continuous_input();

	void draw_game();

	void quit_game();


	void calculate_delta_time(Uint64 frame_start_time);
	void cap_framerate(Uint64 frame_start_time);
	void display_framerate(Uint64 frame_start_time);
	float get_refresh_rate();

	void fire_projectile();

	void insertion_sort_edges(std::vector<Collider::Edge*>& edges);

	void increment_score(int score_increase);


	DisplayFacade game_display_;
	GameState game_state_;
	GameObject object_1_;
	GameObject object_2_;
	//GameObject marker_;
	std::shared_ptr<GameObject> player_;
	GLuint world_border_vao_;
	Camera camera_;
	Texture texture_;

	Skybox skybox_;

	bool player_overlapping_, object_1_overlapping_, object_2_overlapping_;
	int current_score_;


	std::vector<Collider::Edge*> edges_;
	std::set<std::pair<Collider*, Collider*>> overlapping_;


	// Physics Function References.
	void (*add_thrust)(Transform* const, float) = nullptr;
	void (*add_pitch)(Transform* const, float) = nullptr;
	void (*add_yaw)(Transform* const, float) = nullptr;
	void (*add_roll)(Transform* const, float) = nullptr;

	void (*update_physics)(Transform* const, float) = nullptr;

	bool (*check_collisions_radius)(Collider* const, Collider* const) = nullptr;
	bool (*check_collisions_aabb)(Collider* const, Collider* const) = nullptr;

	bool (*sweep_and_prune)(std::vector<Collider::Edge*>& edges, std::set<std::pair<Collider*, Collider*>>&) = nullptr;


	float counter_;
	float delta_time_;
	Uint64 last_frame_start_time_;
	float fixed_time_step_ = 60;	// Default 60 physics updates per second.
};