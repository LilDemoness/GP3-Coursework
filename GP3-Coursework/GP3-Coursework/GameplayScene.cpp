#pragma once

#include "GameplayScene.h"

GameplayScene::GameplayScene() :
	game_state_(GameState::kPlay),
	counter_(0.0f),

	asteroid_spawn_iteration_(0),

	texture_("..\\res\\bricks.jpg"),
	object_1_(std::make_shared<GameObject>("..\\res\\cube1m.obj", Collider::CollisionTag::kUndefined, glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(glm::vec3(45.0f, 45.0f, 0.0f)), glm::vec3(1.0f))),
	object_2_(std::make_shared<GameObject>("..\\res\\cube1m.obj", Collider::CollisionTag::kUndefined, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f))),
	//marker_("..\\res\\monkey3.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.1f)),
	player_(std::make_shared<GameObject>("..\\res\\cube1m.obj", Collider::CollisionTag::kPlayer, glm::vec3(0.0f, 0.0f, 0.0f))),

	camera_(nullptr),
	skybox_()
{
	ShaderManager::get_instance().load_shader("DefaultShader", "..\\res\\shader");
	ShaderManager::get_instance().load_shader("SolidColor", "..\\res\\SolidColourShader")->set_vec3("color", glm::vec3(0.5f));
	ShaderManager::get_instance().set_active_shader("DefaultShader");

	object_1_->set_shader_tag("SolidColor");
	object_2_->set_shader_tag("SolidColor");
	player_->set_shader_tag("SolidColor");


	player_->get_collider()->on_collision_event.subscribe(std::bind(&GameplayScene::on_player_collided, this, std::placeholders::_1, std::placeholders::_2));
	Asteroid::on_any_asteroid_destroyed.subscribe(std::bind(&GameplayScene::increment_score, this, std::placeholders::_1));
	Asteroid::on_all_asteroids_destroyed.subscribe(std::bind(&GameplayScene::prepare_to_respawn_asteroids, this));

	Transform::set_world_radius(PLAY_SPACE_RADIUS);
	WorldBorderVisuals::initialise_world_border(PLAY_SPACE_RADIUS);

	glGenVertexArrays(1, &world_border_vao_);

	InputManager::get_instance().register_input(std::vector<SDL_Keycode>
	{
		// Rotation.
		SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_q, SDLK_e,
		// Movement.
		SDLK_LSHIFT,
	});
	InputManager::get_instance().register_input_event(SDLK_1, std::bind(&GameplayScene::fire_projectile, this));
	InputManager::get_instance().register_input_event(SDLK_5, std::bind(&Asteroid::kill_all_asteroids));
	InputManager::get_instance().register_event(SDL_QUIT, std::bind(&GameplayScene::quit_game, this));

	respawn_asteroids();
}
GameplayScene::~GameplayScene()
{
	player_->get_collider()->on_collision_event.unsubscribe(std::bind(&GameplayScene::on_player_collided, this, std::placeholders::_1, std::placeholders::_2));
	glDeleteVertexArrays(1, &world_border_vao_);

	InputManager::get_instance().deregister_input_event(SDLK_1, std::bind(&GameplayScene::fire_projectile, this));
	InputManager::get_instance().deregister_input_event(SDLK_5, std::bind(&Asteroid::kill_all_asteroids));
	InputManager::get_instance().deregister_event(SDL_QUIT, std::bind(&GameplayScene::quit_game, this));

	Asteroid::on_any_asteroid_destroyed.unsubscribe(std::bind(&GameplayScene::increment_score, this, std::placeholders::_1));
	Asteroid::on_all_asteroids_destroyed.unsubscribe(std::bind(&GameplayScene::prepare_to_respawn_asteroids, this));
	Asteroid::dispose_all();
	Projectile::dispose_all();
}


void GameplayScene::enter(DisplayFacade* display_facade)
{
	// Reset required values.
	score_ = 0;

	// Create our camera.
	camera_ = std::make_unique<Camera>(glm::vec3(0), 70.0f, (float)display_facade->get_width() / display_facade->get_height(), 0.01f, 1000.0f);
	camera_->get_transform()->set_parent(player_->get_transform(), true);
	camera_->get_transform()->set_local_pos(glm::vec3(0.0f, 1.0f, -5.0f));

	// Initialise other required systems.
	init_systems();
}

void GameplayScene::init_systems()
{
	load_dlls();
	init_UBOs();
}
void GameplayScene::init_UBOs()
{
	// Create our UBOs.
	UBOManager::get_instance().create_ubo(kMatricesTag, sizeof(glm::mat4) * 3, 0);

	// Populate with initial data to ensure they aren't undefined.
	const glm::mat4 kIdentity = glm::mat4(1.0f);
	const size_t kMat4Size = sizeof(glm::mat4);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, 0, glm::value_ptr(kIdentity), kMat4Size);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, kMat4Size, glm::value_ptr(kIdentity), kMat4Size);
	UBOManager::get_instance().create_ubo_data(kMatricesTag, kMat4Size * 2, glm::value_ptr(kIdentity), kMat4Size);

	// bind UBO to shaders.
	ShaderManager::get_instance().bind_all_shaders(kMatricesTag);
}

void GameplayScene::load_dlls()
{
	load_physics_engine();
}
void GameplayScene::load_physics_engine()
{
	DLLManager::get_instance().load_dll(PHYSICS_ENGINE_DLL_NAME);
	HelloWorldFunc hello_world = DLLManager::get_instance().get_function<HelloWorldFunc>(PHYSICS_ENGINE_DLL_NAME, "hello_world");

	if (hello_world)
		hello_world();
	else
		std::cerr << "Failed to retrieve the Hello World function from PhysicsDLL" << std::endl;

	add_thrust = DLLManager::get_instance().get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_thrust");
	add_pitch = DLLManager::get_instance().get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_pitch");
	add_yaw = DLLManager::get_instance().get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_yaw");
	add_roll = DLLManager::get_instance().get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_roll");

	update_physics = DLLManager::get_instance().get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "update_physics");

	check_collisions_radius = DLLManager::get_instance().get_function<bool(*)(Collider* const, Collider* const)>(PHYSICS_ENGINE_DLL_NAME, "check_collisions_radius");
	check_collisions_aabb = DLLManager::get_instance().get_function<bool(*)(Collider* const, Collider* const)>(PHYSICS_ENGINE_DLL_NAME, "check_collisions_aabb");

	sweep_and_prune = DLLManager::get_instance().get_function<bool(*)(std::vector<Collider::Edge*>&edges, std::set<std::pair<Collider*, Collider*>>&)>(PHYSICS_ENGINE_DLL_NAME, "sweep_and_prune");
}



void GameplayScene::update(float delta_time)
{
	if (player_death_time_ > 0.0f && counter_ > (player_death_time_ + PLAYER_RESPAWN_TIME))
	{
		on_exit_requested.invoke(kGameOver);
		player_death_time_ = 0.0f;
		return;
	}
	if (spawn_asteroids_time_ > 0.0f && counter_ > spawn_asteroids_time_)
	{
		respawn_asteroids();
		spawn_asteroids_time_ = 0.0f;
	}

	handle_continuous_input(delta_time);

	update_player(delta_time);
	Projectile::update_projectiles(delta_time);
	Asteroid::update_all_asteroids(delta_time);
	skybox_.update(delta_time);

	handle_collisions();

	counter_ += delta_time;
}

void GameplayScene::update_player(float delta_time)
{
	if (!player_)
		return;

	if (update_physics)
	{
		update_physics(player_->get_transform(), delta_time);
	}

	player_->get_transform()->apply_physics(delta_time);
}

void GameplayScene::handle_collisions()
{
	if (sweep_and_prune)
		sweep_and_prune(Collider::Edge::all_edges, overlapping_);


	player_overlapping_ = object_1_overlapping_ = object_2_overlapping_ = false;
	for (std::pair<Collider*, Collider*> overlap : overlapping_)
	{
		if (!overlap.first->get_enabled() || !overlap.second->get_enabled())
			continue;	// One of the colliders is disabled.
		if (!Collider::is_valid_collision(overlap.first, overlap.second))
			continue;	// Invalid collision tags.
		if (!check_collisions_aabb || !check_collisions_aabb(overlap.first, overlap.second))
			continue;	// AABBs aren't overlapping.

		// Valid Collision. Invoke overlap events.
		overlap.first->on_collision_event.invoke(overlap.first, overlap.second);
		overlap.second->on_collision_event.invoke(overlap.second, overlap.first);

		// Temp: Object highlights for overlap.
		if (overlap.first == player_->get_collider() || overlap.second == player_->get_collider()) { player_overlapping_ = true; }
		if (overlap.first == object_1_->get_collider() || overlap.second == object_1_->get_collider()) { object_1_overlapping_ = true; }
		if (overlap.first == object_2_->get_collider() || overlap.second == object_2_->get_collider()) { object_2_overlapping_ = true; }
	}
}


void GameplayScene::quit_game()
{
	on_exit_requested.invoke(Scene::GameMode::kExitGame);
	game_state_ = GameState::kExit;
}
void GameplayScene::handle_continuous_input(float delta_time)
{
	if (!player_)
		return;
	if (!player_->get_is_active())
		return;
	InputManager& instance = InputManager::get_instance();

	const float kPlayerThrust = 5.0f;
	if (instance.get_key_held(SDLK_LSHIFT) && add_thrust)
		add_thrust(player_->get_transform(), kPlayerThrust * delta_time);

	// Rotation.
	constexpr float kPlayerRotationSpeed = glm::radians(360.0f);
	float rotation_speed = kPlayerRotationSpeed * delta_time;
	// Pitch.
	if (instance.get_key_held(SDLK_w) && add_pitch)
		add_pitch(player_->get_transform(), -rotation_speed);
	if (instance.get_key_held(SDLK_s) && add_pitch)
		add_pitch(player_->get_transform(), rotation_speed);
	// Yaw.
	if (instance.get_key_held(SDLK_a) && add_yaw)
		add_yaw(player_->get_transform(), -rotation_speed);
	if (instance.get_key_held(SDLK_d) && add_yaw)
		add_yaw(player_->get_transform(), rotation_speed);
	// Roll.
	if (instance.get_key_held(SDLK_q) && add_roll)
		add_roll(player_->get_transform(), rotation_speed);
	if (instance.get_key_held(SDLK_e) && add_roll)
		add_roll(player_->get_transform(), -rotation_speed);
}


void GameplayScene::draw(DisplayFacade* display_facade)
{
	texture_.bind(0);

	// Render all objects.
	object_1_->draw(*camera_);
	object_2_->draw(*camera_);
	player_->draw(*camera_);
	Asteroid::draw_all(*camera_);
	Projectile::draw_all(*camera_);


	// Render the skybox.
	skybox_.draw(*camera_);

	// Render on-screen text (Score, etc).
	TextRenderer::render_text("Score", display_facade->get_width() / 2.0f, display_facade->get_height() - 50.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(std::to_string(score_), display_facade->get_width() / 2.0f, display_facade->get_height() - 100.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	if (player_->get_is_active())
	{
		// Render the world border only if the player isn't dead.
		WorldBorderVisuals::draw_world_border(player_->get_transform()->get_pos());
	}
}



void GameplayScene::fire_projectile()
{
	Transform* player_transform = player_->get_transform();
	Projectile::spawn_projectile(player_transform->get_pos(), player_transform->get_rot(), player_transform->get_velocity());
}


void GameplayScene::increment_score(int score_increase)
{
	score_ += score_increase;
}
void GameplayScene::on_player_collided(Collider* player, Collider* other)
{
	if (other->get_collision_tag() == Collider::CollisionTag::kAsteroid)
	{
		player_->set_is_active(false);
		player_->get_transform()->set_ignore_bounds(true);
		player_death_time_ = counter_;

		camera_->get_transform()->clear_parent();
	}
}


void GameplayScene::insertion_sort_edges(std::vector<Collider::Edge*>& edges)
{
	for (unsigned int i = 1; i < edges.size(); ++i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			if (edges[j]->get_x_position() < edges[j + 1]->get_x_position())
				break;

			// Sort.
			std::iter_swap(edges.begin() + j, edges.begin() + j + 1);
		}
	}
}



void GameplayScene::prepare_to_respawn_asteroids()
{
	spawn_asteroids_time_ = counter_ + 1.5f;
}
void GameplayScene::respawn_asteroids()
{
	const int kBaseAsteroidSpawns = 10;
	const int kBaseAsteroidSplits = 2;
	const float kBaseMinSpeed = 0.2f;
	const float kBaseMaxSpeed = 3.0f;

	float scaling_factor = std::powf(1.5f, asteroid_spawn_iteration_);
	Asteroid::create_initial_asteroids((int)(kBaseAsteroidSpawns * scaling_factor), kBaseAsteroidSplits + (int)(asteroid_spawn_iteration_ / 3), kBaseMinSpeed * scaling_factor, kBaseMaxSpeed * scaling_factor, PLAY_SPACE_RADIUS - 2.5f);	// We're subtracting a delta for the spawn radius to prevent spawning an asteroid which immediately moves out of the world border.

	++asteroid_spawn_iteration_;
}