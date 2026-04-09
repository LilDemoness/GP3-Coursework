#pragma once

#include "MainGame.h"

MainGame::MainGame() :
	game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
	game_state_(GameState::kPlay),
	counter_(0.0f),
	delta_time_(0.0f),
	last_frame_start_time_(0.0f),

    texture_("..\\res\\bricks.jpg"),
    //object_1_(Mesh::create_triangle_mesh()),
    //object_2_("..\\res\\monkey3.obj", glm::vec3(2.0f, 0.0f, 0.0f)),
    object_1_("..\\res\\cube1m.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(glm::vec3(45.0f, 45.0f, 0.0f)), glm::vec3(1.0f), 0.5f),
    object_2_("..\\res\\cube1m.obj", glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 0.5f),
    marker_("..\\res\\monkey3.obj", glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.1f)),
    //player_(std::make_shared<GameObject>(Mesh::create_triangle_mesh(), glm::vec3(0.0f, 0.0f, 0.0f))),
    player_(std::make_shared<GameObject>("..\\res\\cube1m.obj", glm::vec3(0.0f, 0.0f, 0.0f))),
    //player_(std::make_shared<GameObject>("..\\res\\monkey3.obj", glm::vec3(0.0f, 0.0f, 0.0f))),

	//projectiles_pool_test_(create_projectile),

	camera_(glm::vec3(0), 70.0f, (float)game_display_.get_width() / game_display_.get_height(), 0.01f, 1000.0f)
{
	ShaderManager::get_instance().load_shader("DefaultShader", "..\\res\\shader");
	ShaderManager::get_instance().load_shader("SolidColor", "..\\res\\SolidColourShader");

	fixed_time_step_ = 1.0f / get_refresh_rate();

	InputManager::get_instance().register_input(std::vector<SDL_Keycode>
	{
		// Rotation.
		SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_q, SDLK_e,
		// Movement.
		SDLK_LSHIFT,
	});
	InputManager::get_instance().register_input_event(SDLK_1, std::bind(&MainGame::fire_projectile, this));
	InputManager::get_instance().register_input_event(SDLK_2, std::bind(&MainGame::release_projectiles, this));

	//projectiles_pool_test_(ObjectPool<GameObject>(std::bind(&MainGame::callback_test, this))),
	projectiles_pool_test_ = ObjectPool<GameObject>(std::bind(&MainGame::create_projectile, this), std::bind(&MainGame::on_get_projectile, this, std::placeholders::_1), std::bind(&MainGame::on_release_projectile, this, std::placeholders::_1), nullptr);
	//projectiles_pool_test_.register_callback<MainGame>(this, &MainGame::callback_test);

	edges_ = std::vector<Edge*>
	{
		new Edge(object_1_.get_collider(), true),	new Edge(object_1_.get_collider(), false),
		new Edge(object_2_.get_collider(), true),	new Edge(object_2_.get_collider(), false),
		new Edge(player_->get_collider(), true),	new Edge(player_->get_collider(), false),
	};

	camera_.get_transform()->set_parent(player_->get_transform(), true);
	camera_.get_transform()->set_local_pos(glm::vec3(0.0f, 0.0f, -5.0f));
}
MainGame::~MainGame()
{
	ShaderManager::get_instance().clear();

	InputManager::get_instance().deregister_input_event(SDLK_1, std::bind(&MainGame::fire_projectile, this));
	InputManager::get_instance().deregister_input_event(SDLK_2, std::bind(&MainGame::release_projectiles, this));

	for (std::shared_ptr<GameObject> val : active_projectiles_)
		projectiles_pool_test_.release(val);
	active_projectiles_.clear();
}


void MainGame::Run()
{
	init_systems(); 
	game_loop();
}

void MainGame::init_systems()
{
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering.
	glEnable(GL_CULL_FACE);  // Enable face culling.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black.

	load_dlls();
	init_UBOs();
}
void MainGame::init_UBOs()
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

void MainGame::load_dlls()
{
	load_physics_engine();
}
void MainGame::load_physics_engine()
{
	DLLManager::get_instance().load_dll(PHYSICS_ENGINE_DLL_NAME);
	HelloWorldFunc hello_world = DLLManager::get_instance().get_function<HelloWorldFunc>(PHYSICS_ENGINE_DLL_NAME, "hello_world");
	//HelloWorldFunc hello_world = DLLManager::get_instance().get_function<HelloWorldFunc>(PHYSICS_ENGINE_DLL_NAME, "HelloWorld");

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

	sweep_and_prune = DLLManager::get_instance().get_function<bool(*)(std::vector<Edge*>&edges, std::set<std::pair<Collider*, Collider*>>&)>(PHYSICS_ENGINE_DLL_NAME, "sweep_and_prune");
}



void MainGame::game_loop()
{
	while (game_state_ != GameState::kExit)
	{
		// Cache the high resolution time at the start of the frame.
		Uint64 start = SDL_GetPerformanceCounter();
		calculate_delta_time(start);


		// Handle the frame.
		process_input_events();
		process_input();

		//player_->get_transform()->rotate(glm::radians(glm::vec3(45.0f, 45.0f, 0.0f) * delta_time_));
		//camera_.get_transform()->rotate_around_point(glm::vec3(0.0f), glm::radians(glm::vec3(180.0f, 0.0f, 0.0f) * delta_time_));
		//object_1_.get_transform()->rotate(glm::vec3(0.25f, 1.0f, 0.0f), glm::radians(45.0f) * delta_time_);
		object_2_.get_transform()->set_pos(glm::vec3(glm::sin(glm::radians(counter_ * 45.0f + 270.0f)) * 3.0f, 0.75f, 0.5f));
		update_player();

		if (sweep_and_prune)
			sweep_and_prune(edges_, overlapping_);

		draw_game();


		// Limit and (optionally) display our framerate.
		counter_ += delta_time_;
		cap_framerate(start);
		//DisplayFramerate(start);
	}
}

void MainGame::update_player()
{
	if (!player_)
		return;

	if (update_physics)
	{
		update_physics(player_->get_transform(), delta_time_);
		for (auto projectile : active_projectiles_)
			update_physics(projectile->get_transform(), delta_time_);
	}

	player_->get_transform()->apply_physics(delta_time_);

	for (auto projectile : active_projectiles_)
		projectile->get_transform()->apply_physics(delta_time_);
}


void MainGame::process_input_events()
{
	// Get and process events
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			game_state_ = GameState::kExit;
			break;
		default:
			InputManager::get_instance().process_input_event(evnt);
			break;
		}
	}

	InputManager::get_instance().process_general_input();
}
void MainGame::process_input()
{
	if (!player_)
		return;
	InputManager& instance = InputManager::get_instance();

	const float kPlayerThrust = 5.0f;
	if (instance.get_key_held(SDLK_LSHIFT) && add_thrust)
		add_thrust(player_->get_transform(), kPlayerThrust * delta_time_);

	// Rotation.
	const float kPlayerRotationSpeed = glm::radians(360.0f);
	float rotation_speed = kPlayerRotationSpeed * delta_time_;
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


void MainGame::draw_game()
{
	game_display_.clear_display();

	std::shared_ptr<Shader> override_shader = ShaderManager::get_instance().get_shader("SolidColor");
	texture_.bind(0);

	/*if (check_collisions_aabb && check_collisions_aabb(object_1_.get_collider(), object_2_.get_collider()))
		override_shader->set_vec3("color", glm::vec3(1.0f));
	else
		override_shader->set_vec3("color", glm::vec3(0.5f));
	object_1_.draw(camera_, override_shader);

	if (check_collisions_aabb && check_collisions_aabb(object_1_.get_collider(), object_2_.get_collider()))
		override_shader->set_vec3("color", glm::vec3(1.0f));
	else
		override_shader->set_vec3("color", glm::vec3(0.5f));
	object_2_.draw(camera_, override_shader);*/

	bool player_overlapped = false, obj_1_overlapped = false, obj_2_overlapped = false;
	for (std::pair<Collider*, Collider*> overlap : overlapping_)
	{
		if (!check_collisions_aabb || !check_collisions_aabb(overlap.first, overlap.second))
			continue;
		if (!overlap.first->get_enabled() || !overlap.second->get_enabled())
			continue;

		if (overlap.first == player_->get_collider() || overlap.second == player_->get_collider())
			player_overlapped = true;
		if (overlap.first == object_1_.get_collider() || overlap.second == object_1_.get_collider())
			obj_1_overlapped = true;
		if (overlap.first == object_2_.get_collider() || overlap.second == object_2_.get_collider())
			obj_2_overlapped = true;
	}
	override_shader->set_vec3("color", glm::vec3((1.0f * obj_1_overlapped) / 2.0f + 0.5f));
	object_1_.draw(camera_, override_shader);

	override_shader->set_vec3("color", glm::vec3((1.0f * obj_2_overlapped) / 2.0f + 0.5f));
	object_2_.draw(camera_, override_shader);

	override_shader->set_vec3("color", glm::vec3((1.0f * player_overlapped) / 2.0f + 0.5f));
	player_->draw(camera_, override_shader);

	for (auto val : active_projectiles_)
		val->draw(camera_);

	int modulus = static_cast<int>(floorf(counter_ / 2.0f)) % 8;
	glm::vec3 half_extents = object_1_.get_collider()->get_aabb_half_extents();
	marker_.get_transform()->set_pos(glm::vec3(half_extents.x, half_extents.y, -half_extents.z));
	/*switch (modulus)
	{
	case 0: marker_.get_transform()->set_pos(glm::vec3(half_extents.x, half_extents.y, -half_extents.z)); break;
	case 1: marker_.get_transform()->set_pos(glm::vec3(-half_extents.x, half_extents.y, -half_extents.z)); break;
	case 2: marker_.get_transform()->set_pos(glm::vec3(half_extents.x, -half_extents.y, -half_extents.z)); break;
	case 3: marker_.get_transform()->set_pos(glm::vec3(-half_extents.x, -half_extents.y, -half_extents.z)); break;
	case 4: marker_.get_transform()->set_pos(glm::vec3(half_extents.x, half_extents.y, half_extents.z)); break;
	case 5: marker_.get_transform()->set_pos(glm::vec3(-half_extents.x, half_extents.y, half_extents.z)); break;
	case 6: marker_.get_transform()->set_pos(glm::vec3(half_extents.x, -half_extents.y, half_extents.z)); break;
	case 7: marker_.get_transform()->set_pos(glm::vec3(-half_extents.x, -half_extents.y, half_extents.z)); break;
	}*/
	marker_.draw(camera_);

				
	//glEnableClientState(GL_COLOR_ARRAY); 
	//glEnd();

	game_display_.swap_buffers();
}



void MainGame::calculate_delta_time(Uint64 frame_start_time)
{
	delta_time_ = (frame_start_time - last_frame_start_time_) / (float)SDL_GetPerformanceFrequency();
	last_frame_start_time_ = frame_start_time;
}
void MainGame::cap_framerate(Uint64 frame_start_time)
{
	// Cache the high resolution time after handling our frame.
	Uint64 frame_end_time = SDL_GetPerformanceCounter();
	float elapsed_ms = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

	// Cap our framerate.
	// Note: Can be innaccurate by a small amount for high framerates (E.g. 120FPS).
	const float kDelayMS = (1.0f / (float)MAX_FRAMERATE) * 1000.0f;
	float duration = std::floor(kDelayMS - elapsed_ms);
	if (duration > 0)
	{
		SDL_Delay(duration);
	}
}
void MainGame::display_framerate(Uint64 frame_start_time)
{
	// Output our Framerate.
	Uint64 frame_end_time = SDL_GetPerformanceCounter();
	float elapsed_s = (frame_end_time - frame_start_time) / (float)SDL_GetPerformanceFrequency();
	std::cout << "Current FPS: " << std::to_string(std::round(1.0 / elapsed_s)) << ". Delta Time: " << std::to_string(delta_time_) << std::endl;
}


float MainGame::get_refresh_rate()
{
	SDL_DisplayMode display_mode;
	if (SDL_GetCurrentDisplayMode(0, &display_mode) == 0 && display_mode.refresh_rate > 0)
	{
		return static_cast<float>(display_mode.refresh_rate);
	}

	const float kDefaultRefreshRate = 60.0f;
	std::cout << "Failed to retrieve monitor Refresh Rate. Defaulting to " << kDefaultRefreshRate << std::endl;
	return kDefaultRefreshRate;
}


std::shared_ptr<GameObject> MainGame::create_projectile()
{
	std::shared_ptr<GameObject> new_instance = std::make_shared<GameObject>("..\\res\\cube1m.obj", glm::vec3(0.0f), glm::quat(), glm::vec3(0.2f), 0.1f);

	edges_.emplace(edges_.end(), new Edge(new_instance->get_collider(), true));
	edges_.emplace(edges_.end(), new Edge(new_instance->get_collider(), false));

	return new_instance;
}
void MainGame::on_get_projectile(std::shared_ptr<GameObject> projectile_instance)
{
	// Set Position.
	Transform* player_transform = player_->get_transform();
	projectile_instance->get_transform()->set_pos(player_transform->get_pos());
	projectile_instance->get_transform()->set_rot(player_transform->get_rot());

	// Enable Collisions.
	projectile_instance->get_collider()->set_enabled(true);

	// Apply initial force.
	if (add_thrust)
		add_thrust(projectile_instance->get_transform(), 15.0f);
}
void MainGame::on_release_projectile(std::shared_ptr<GameObject> projectile_instance)
{
	// Disable Collisions.
	projectile_instance->get_collider()->set_enabled(false);

	// Remove from Active Projectiles list (Prevents rendering & updating).
	int index = -1;
	for (int i = 0; i < active_projectiles_.size(); ++i)
	{
		if (active_projectiles_[i] == projectile_instance)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		std::cerr << "Failed to find projectile to erase" << std::endl;
		return;
	}

	active_projectiles_.erase(active_projectiles_.begin() + index);
}

void MainGame::fire_projectile()
{
	active_projectiles_.emplace(active_projectiles_.end(), projectiles_pool_test_.get());
}
void MainGame::release_projectiles()
{
	for (int i = active_projectiles_.size() - 1; i >= 0; --i)
		projectiles_pool_test_.release(active_projectiles_[i]);
}



void MainGame::insertion_sort_edges(std::vector<Edge*>& edges)
{
	for (int i = 1; i < edges.size(); ++i)
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