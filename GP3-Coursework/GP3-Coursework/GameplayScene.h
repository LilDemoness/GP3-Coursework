#pragma once
#include "Scene.h"

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


constexpr float kPlaySpaceRadius = 25.0f;

constexpr float kPlayerRespawnTime = 3.0f;

#define PHYSICS_ENGINE_DLL_NAME "PhysicsEngine.dll"
using HelloWorldFunc = void(*)();

enum class GameState{kPlay, kExit};

class GameplayScene : public Scene
{
public:
	GameplayScene(DisplayFacade* display_facade);
	~GameplayScene();

	inline GameMode get_game_mode() override { return kGameplay; }


	void enter(DisplayFacade* display_facade) override;
	void update(float delta_time) override;
	void draw(DisplayFacade* display_facade) override;
	void on_exit_fulfilled() override {}

private:
	void init_systems();
	void init_UBOs();

	void load_dlls();
	void load_physics_engine();


	void handle_collisions();

	void handle_continuous_input(float delta_time);


	void draw_black_hole(DisplayFacade* display_facade);


	void fire_projectile();

	void on_player_collided(Collider* player, Collider* other);
	void increment_score(int score_increase);


	GameState game_state_;
	std::shared_ptr<GameObject> player_;
	std::shared_ptr<GameObject> centre_indicator_;
	GLuint world_border_vao_;
	std::unique_ptr<Camera> camera_;

	std::shared_ptr<Texture> black_hole_noise_texture_;
	std::shared_ptr<Texture> black_hole_opaque_texture_;

	Skybox skybox_;

	bool player_overlapping_, object_1_overlapping_, object_2_overlapping_;

	float player_death_time_; // <0 = unset;


	// Asteroids.
	float spawn_asteroids_time_; // <0 = unset;
	int asteroid_spawn_iteration_;

	void prepare_to_respawn_asteroids();
	void respawn_asteroids();


	std::vector<Collider::Edge*> edges_;
	std::set<std::pair<Collider*, Collider*>> overlapping_;


	// Physics Function References.
	void (*add_thrust)(Transform* const, float) = nullptr;
	void (*add_pitch)(Transform* const, float) = nullptr;
	void (*add_yaw)(Transform* const, float) = nullptr;
	void (*add_roll)(Transform* const, float) = nullptr;

	void (*update_physics)(Transform* const, float, bool, float) = nullptr;
	void (*apply_physics)(Transform* const, float, float) = nullptr;

	bool (*check_collisions)(Collider* const, Collider* const) = nullptr;

	bool (*sweep_and_prune)(std::vector<Collider::Edge*>& edges, std::set<std::pair<Collider*, Collider*>>&) = nullptr;


	float counter_;
};