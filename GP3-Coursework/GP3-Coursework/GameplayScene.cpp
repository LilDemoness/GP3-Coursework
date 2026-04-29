#pragma once

#include "GameplayScene.h"

GameplayScene::GameplayScene() :
	game_state_(GameState::kPlay),
	counter_(0.0f),

	asteroid_spawn_iteration_(0),

	//centre_indicator_(std::make_shared<GameObject>(Mesh::create_mesh("..\\res\\IcoSphere1m.obj"), Texture::create_texture("..\\res\\WhitePixel.jpg"), Collider::CollisionTag::kUndefined, glm::vec3(0.0f), glm::quat(), glm::vec3(0.25f))),
	centre_indicator_(std::make_shared<GameObject>(Mesh::create_mesh("..\\res\\IcoSphere1m.obj"), Texture::create_texture("..\\res\\WhitePixel.jpg"), Collider::CollisionTag::kUndefined, glm::vec3(0.0f), glm::quat(), glm::vec3(2.0f, 5.0f, 5.0f))),
	player_(std::make_shared<GameObject>(Mesh::create_mesh("..\\res\\PlayerModel.obj"), Texture::create_texture("..\\res\\bricks.jpg"), Collider::CollisionTag::kPlayer, glm::vec3(0.0f, 0.0f, 0.0f))),

	black_hole_noise_texture_(Texture::create_texture("..\\res\\NoiseTexture.png")),

	camera_(nullptr),
	skybox_()
{
	ShaderManager::load_shader("DefaultShader", "..\\res\\shader");
	ShaderManager::load_shader("PlayerDeathShader", "..\\res\\explode.vert", "..\\res\\explode.geom", "..\\res\\shader.frag");
	ShaderManager::load_shader("SolidColor", "..\\res\\SolidColourShader")->set_vec3("color", glm::vec3(0.5f));
	ShaderManager::load_shader("BlackHole", "..\\res\\Shaders\\BlackHoleShader");
	ShaderManager::set_active_shader("DefaultShader");

	centre_indicator_->set_shader_tag("BlackHole");


	player_->get_collider()->on_collision_event.subscribe(std::bind(&GameplayScene::on_player_collided, this, std::placeholders::_1, std::placeholders::_2));
	Asteroid::on_any_asteroid_destroyed.subscribe(std::bind(&GameplayScene::increment_score, this, std::placeholders::_1));
	Asteroid::on_all_asteroids_destroyed.subscribe(std::bind(&GameplayScene::prepare_to_respawn_asteroids, this));

	Transform::set_world_radius(kPlaySpaceRadius);
	WorldBorderVisuals::initialise_world_border(kPlaySpaceRadius);

	glGenVertexArrays(1, &world_border_vao_);

	InputManager::register_input(std::vector<SDL_Keycode>
	{
		// Rotation.
		SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_q, SDLK_e,
		// Movement.
		SDLK_LSHIFT,
	});
	InputManager::register_input_event(SDLK_SPACE, std::bind(&GameplayScene::fire_projectile, this));
	InputManager::register_input_event(SDLK_5, std::bind(&Asteroid::kill_all_asteroids));
	InputManager::register_event(SDL_QUIT, std::bind(&GameplayScene::quit_game, this));

	respawn_asteroids();
}
GameplayScene::~GameplayScene()
{
	player_->get_collider()->on_collision_event.unsubscribe(std::bind(&GameplayScene::on_player_collided, this, std::placeholders::_1, std::placeholders::_2));
	glDeleteVertexArrays(1, &world_border_vao_);

	InputManager::deregister_input_event(SDLK_1, std::bind(&GameplayScene::fire_projectile, this));
	InputManager::deregister_input_event(SDLK_5, std::bind(&Asteroid::kill_all_asteroids));
	InputManager::deregister_event(SDL_QUIT, std::bind(&GameplayScene::quit_game, this));

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
	UBOManager::create_ubo(kMatricesTag, sizeof(glm::mat4) * 2, 0);

	// Populate with initial data to ensure they aren't undefined.
	const glm::mat4 kIdentity = glm::mat4(1.0f);
	const size_t kMat4Size = sizeof(glm::mat4);
	UBOManager::create_ubo_data(kMatricesTag, 0, glm::value_ptr(kIdentity), kMat4Size);
	UBOManager::create_ubo_data(kMatricesTag, kMat4Size, glm::value_ptr(kIdentity), kMat4Size);

	// bind UBO to shaders.
	ShaderManager::bind_all_shaders(kMatricesTag);
}

void GameplayScene::load_dlls()
{
	load_physics_engine();
}
void GameplayScene::load_physics_engine()
{
	DLLManager::load_dll(PHYSICS_ENGINE_DLL_NAME);
	HelloWorldFunc hello_world = DLLManager::get_function<HelloWorldFunc>(PHYSICS_ENGINE_DLL_NAME, "hello_world");

	if (hello_world)
		hello_world();
	else
		std::cerr << "Failed to retrieve the Hello World function from PhysicsDLL" << std::endl;

	add_thrust = DLLManager::get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_thrust");
	add_pitch = DLLManager::get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_pitch");
	add_yaw = DLLManager::get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_yaw");
	add_roll = DLLManager::get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "add_roll");

	update_physics = DLLManager::get_function<void(*)(Transform* const, float)>(PHYSICS_ENGINE_DLL_NAME, "update_physics");

	check_collisions_radius = DLLManager::get_function<bool(*)(Collider* const, Collider* const)>(PHYSICS_ENGINE_DLL_NAME, "check_collisions_radius");
	check_collisions_aabb = DLLManager::get_function<bool(*)(Collider* const, Collider* const)>(PHYSICS_ENGINE_DLL_NAME, "check_collisions_aabb");

	sweep_and_prune = DLLManager::get_function<bool(*)(std::vector<Collider::Edge*>&edges, std::set<std::pair<Collider*, Collider*>>&)>(PHYSICS_ENGINE_DLL_NAME, "sweep_and_prune");
}



void GameplayScene::update(float delta_time)
{
	if (player_death_time_ > 0.0f)
	{
		ShaderManager::get_shader("PlayerDeathShader")->set_float("explosion_percentage", (counter_ - player_death_time_) / kPlayerRespawnTime);

		if (counter_ > (player_death_time_ + kPlayerRespawnTime))
		{
			on_exit_requested.invoke(kGameOver);
			player_death_time_ = 0.0f;
			return;
		}
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

	const float kPlayerThrust = 5.0f;
	if (InputManager::get_key_held(SDLK_LSHIFT) && add_thrust)
		add_thrust(player_->get_transform(), kPlayerThrust * delta_time);

	// Rotation.
	constexpr float kPlayerRotationSpeed = glm::radians(360.0f);
	float rotation_speed = kPlayerRotationSpeed * delta_time;
	// Pitch.
	if (InputManager::get_key_held(SDLK_w) && add_pitch)
		add_pitch(player_->get_transform(), -rotation_speed);
	if (InputManager::get_key_held(SDLK_s) && add_pitch)
		add_pitch(player_->get_transform(), rotation_speed);
	// Yaw.
	if (InputManager::get_key_held(SDLK_a) && add_yaw)
		add_yaw(player_->get_transform(), -rotation_speed);
	if (InputManager::get_key_held(SDLK_d) && add_yaw)
		add_yaw(player_->get_transform(), rotation_speed);
	// Roll.
	if (InputManager::get_key_held(SDLK_q) && add_roll)
		add_roll(player_->get_transform(), rotation_speed);
	if (InputManager::get_key_held(SDLK_e) && add_roll)
		add_roll(player_->get_transform(), -rotation_speed);
}


void GameplayScene::draw(DisplayFacade* display_facade)
{
	// Render all objects.
	if (player_death_time_ > 0.0f)
	{
		// Disable backface culling for the player when rendering them in their death animation, allowing us to see their inner faces while exploding.
		display_facade->set_cull_backface(false);
		player_->draw(*camera_);
		display_facade->set_cull_backface(true);
	}
	else
		player_->draw(*camera_);

	Asteroid::draw_all(*camera_);
	Projectile::draw_all(*camera_);


	// Render the skybox.
	skybox_.draw(*camera_);

	// Render on-screen text (Score, etc).
	TextRenderer::render_text("Score", display_facade->get_width() / 2.0f, display_facade->get_height() - 50.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(std::to_string(score_), display_facade->get_width() / 2.0f, display_facade->get_height() - 100.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	
	std::shared_ptr<Shader> black_hole_shader = ShaderManager::get_shader("BlackHole");
	black_hole_shader->set_vec3("camera_pos_ws", camera_->get_transform()->get_pos(), true);
	black_hole_shader->set_vec3("disc_normal", centre_indicator_->get_transform()->get_up(), true);
	black_hole_shader->set_vec3("black_hole_centre", centre_indicator_->get_transform()->get_pos(), true);
	black_hole_shader->set_float("black_hole_radius", centre_indicator_->get_transform()->get_scale().x / 2.0f, true);
	black_hole_shader->set_vec2("screen_size", glm::vec2(display_facade->get_width(), display_facade->get_height()), true);
	black_hole_shader->set_float("time", counter_);

	display_facade->bind_screen_texture();
	black_hole_noise_texture_->bind(1);
	black_hole_shader->set_int("screen_texture", 0);
	black_hole_shader->set_int("disc_texture", 1);
	centre_indicator_->draw(*camera_, false);

	glBindTexture(GL_TEXTURE_2D, 0);


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

		player_->set_shader_tag("PlayerDeathShader");

		camera_->get_transform()->clear_parent();
	}
}



void GameplayScene::prepare_to_respawn_asteroids()
{
	spawn_asteroids_time_ = counter_ + 1.5f;
}
void GameplayScene::respawn_asteroids()
{
	// Asteroid Spawning Settings (Base values are unchanged for iteration 0).
	constexpr int kBaseAsteroidSpawns = 10;

	constexpr int kBaseAsteroidSplits = 2;
	constexpr int kMaxAsteroidSplits = 3;			// Maximum number of splits an asteroid can have.

	constexpr float kBaseMinSpeed = 0.2f;
	//constexpr float kMinSpeedIncreaseRate = 0.0f;	// How much the minimum asteroid speed increases by every spawn iteration.

	constexpr float kBaseMaxSpeed = 3.0f;
	//constexpr float kMaxSpeedIncreaseRate = 0.0f;	// How much the maximum asteroid speed increases by every spawn iteration.

	constexpr float kSpawnRadius = kPlaySpaceRadius - 2.5f;	// We're subtracting a delta for the spawn radius to prevent spawning an asteroid which immediately moves out of the world border.
	constexpr float kScalingRate = 1.25f;

	// Calculate our desired values for this spawn iteration.
	float scaling_factor = std::powf(kScalingRate, (float)asteroid_spawn_iteration_);
	int spawns = (int)std::ceilf(kBaseAsteroidSpawns * scaling_factor);
	int splits = asteroid_spawn_iteration_ < 3 ? kBaseAsteroidSplits : kMaxAsteroidSplits;//(int)std::fmin(kBaseAsteroidSplits + asteroid_spawn_iteration_ / 3.0f, kMaxAsteroidSplits);
	//float min_speed = kBaseMinSpeed + kMinSpeedIncreaseRate * asteroid_spawn_iteration_;
	//float max_speed = kBaseMaxSpeed + kMaxSpeedIncreaseRate * asteroid_spawn_iteration_;

	// Respawn the asteroids.
	Asteroid::create_initial_asteroids(spawns, splits, kBaseMinSpeed, kBaseMaxSpeed, kSpawnRadius);
	++asteroid_spawn_iteration_;
}