#pragma once
#include "Asteroid.h"


Asteroid::Asteroid()
	: GameObject(Mesh::create_mesh(ASTEROIDS_MODEL_PATH, kMaxAsteroids), Collider::CollisionTag::kAsteroid, glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f), true),
	remaining_splits_(0)
{
	collider_->on_collision_event.subscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));
}
Asteroid::Asteroid(glm::vec3 position, glm::quat rot, glm::vec3 scale, int remaining_splits)
	: GameObject(Mesh::create_mesh(ASTEROIDS_MODEL_PATH, kMaxAsteroids), Collider::CollisionTag::kAsteroid, position, rot, scale, true),
	remaining_splits_(remaining_splits)
{
	collider_->on_collision_event.subscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));
}
Asteroid::~Asteroid()
{
	collider_->on_collision_event.unsubscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));

	/*for (auto it = all_active_asteroids_.begin(); it != all_active_asteroids_.end(); ++it)
		if (it->get() == this)
			it = all_active_asteroids_.erase(it);*/
}
void Asteroid::dispose_all()
{
	all_active_asteroids_.clear();
	asteroids_pool_.clear();
}


void Asteroid::draw_all(const Camera& camera)
{
	//draw_all(camera, shader_tag_.empty() ? ShaderManager::get_active_shader() : ShaderManager::get_shader(shader_tag_));
	draw_all(camera, ShaderManager::get_active_shader());
}
void Asteroid::draw_all(const Camera& camera, std::shared_ptr<Shader> shader)
{
	if (all_active_asteroids_.empty())
		return;

	shader->bind();
	shader->update_matrices_ubo(camera);

	Mesh* asteroid_mesh = all_active_asteroids_.begin()->get()->get_mesh();

	// Bind all model matrices.
	int i = 0;
	asteroid_mesh->bind_vao();
	for (const std::shared_ptr<Asteroid> asteroid : all_active_asteroids_)
	{
		if (asteroid->get_is_active())
		{
			asteroid_mesh->set_instance_matrix(i, asteroid->get_transform()->get_model());
			i++;
		}
	}

	// Render.
	asteroid_mesh->draw_instanced(all_active_asteroids_.size());
}



// ----- Spawning -----

void Asteroid::create_initial_asteroids(const int asteroids_count, const int asteroid_split_count, const float min_velocity, const float max_velocity, const float world_bounds)
{
	const glm::vec3 kForward = glm::vec3(0.0f, 1.0f, 0.0f);

	// Setup randomness.
	constexpr float kFloatResolution = 10.0f;
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> random_velocity_magnitude(min_velocity * kFloatResolution, max_velocity * kFloatResolution);


	for (int i = 0; i < asteroids_count; ++i)
	{
		std::shared_ptr<Asteroid> asteroid = asteroids_pool_.get();

		glm::vec3 position = world_bounds * get_random_direction_vector(gen);
		glm::quat rotation = get_random_direction(gen);
		float scale = get_scale_for_remaining_splits(asteroid_split_count);

		glm::vec3 velocity = (random_velocity_magnitude(gen) / (float)kFloatResolution) * (rotation * kForward);
		glm::vec3 angular_velocity = glm::radians(15.0f * kForward * rotation);

		asteroid->setup_asteroid(asteroid_split_count, position, rotation, scale, velocity, angular_velocity);
	}
}
void Asteroid::setup_asteroid(int remaining_splits, glm::vec3 pos, glm::quat rot, float scale, glm::vec3 velocity, glm::vec3 angular_velocity)
{
	transform_->set_pos(pos);
	transform_->set_rot(rot);
	transform_->set_scale(glm::vec3(scale));

	// HalfExtents & Radius are already affected by scale.
	//collider_->set_radius(scale / 2.0f);

	transform_->set_velocity(velocity);
	transform_->set_angular_velocity(angular_velocity);


	remaining_splits_ = remaining_splits;
}
glm::vec3 Asteroid::get_random_direction_vector(std::mt19937 gen)
{
	const int kFloatResolution = 10;
	std::uniform_int_distribution<> random_float(-kFloatResolution, kFloatResolution);
	return glm::normalize(glm::vec3(random_float(gen), random_float(gen), random_float(gen)));
}
glm::quat Asteroid::get_random_direction(std::mt19937 gen)
{
	std::uniform_int_distribution<> random_angle(0, 359);
	return glm::quat(glm::radians(glm::vec3(random_angle(gen), random_angle(gen), random_angle(gen))));
}


void Asteroid::kill_all_asteroids()
{
	for (auto it = all_active_asteroids_.begin(); it != all_active_asteroids_.end();)
	{
		const std::shared_ptr<Asteroid> current = *it++;

		current->remaining_splits_ = 0;
		current->on_collision(current->get_collider(), nullptr);
	}
}

Event<int> Asteroid::on_any_asteroid_destroyed;
Event<> Asteroid::on_all_asteroids_destroyed;
void Asteroid::on_collision(Collider* self, Collider* other)
{
	on_any_asteroid_destroyed.invoke(get_score_for_size(remaining_splits_));
	split();

	if (all_active_asteroids_.empty())
	{
		on_all_asteroids_destroyed.invoke();
	}
}
void Asteroid::split()
{
	if (remaining_splits_ <= 0)
	{
		for (const std::shared_ptr<Asteroid> asteroid : all_active_asteroids_)
		{
			if (asteroid.get() == this)
			{
				asteroids_pool_.release(asteroid);
				break;
			}
		}
		std::cout << "Out of Splits" << std::endl;
		return;
	}


	// Setup randomness.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> random(0, 359);


	// Calculate shared values.
	const int kSplitAsteroidsCount = 2;
	int new_remaining_splits = remaining_splits_ - 1;
	float new_scale = get_scale_for_remaining_splits(new_remaining_splits);

	glm::vec3 pos = transform_->get_pos();

	const glm::vec3 kForward = glm::vec3(0.0f, 1.0f, 0.0f);
	float base_velocity_magnitude = transform_->get_velocity().length();


	// Have this asteroid be one of the halves.
	glm::quat rot = get_random_direction(gen);
	glm::vec3 angular_velocity = glm::eulerAngles(rot);
	this->setup_asteroid(new_remaining_splits, pos, rot, new_scale, kForward * rot * base_velocity_magnitude * 1.5f, angular_velocity);

	// Create the other halves.
	for (int i = 1; i < kSplitAsteroidsCount; ++i)
	{
		std::shared_ptr<Asteroid> asteroid = asteroids_pool_.get();

		rot = get_random_direction(gen);
		glm::vec3 velocity = base_velocity_magnitude * 1.5f * (rot * kForward);
		angular_velocity = glm::eulerAngles(rot);
		asteroid->setup_asteroid(new_remaining_splits, pos, rot, new_scale, velocity, angular_velocity);
	}
}


float Asteroid::get_scale_for_remaining_splits(int remaining_splits)
{
	/*
	// Increases linearly.
	const float kScaleIncrement = 0.5f;
	const float kBaseScale = 0.5f;
	return (remaining_splits * kScaleIncrement) + kBaseScale;*/

	// Doubles each time.
	const float kBaseScale = 0.35f;
	return std::powf(2, (float)remaining_splits) * kBaseScale;
}


int Asteroid::get_score_for_size(int remaining_splits)
{
	const int SMALL_ASTEROID_SCORE = 200;
	const float SCORE_DECREASE_RATE = 2.0f;
	return (int)(SMALL_ASTEROID_SCORE / std::powf(SCORE_DECREASE_RATE, (float)remaining_splits));
}


std::unordered_set<std::shared_ptr<Asteroid>> Asteroid::all_active_asteroids_ = std::unordered_set<std::shared_ptr<Asteroid>>();
void Asteroid::update_all_asteroids(float delta_time)
{
	for (auto it = all_active_asteroids_.begin(); it != all_active_asteroids_.end(); ++it)
		(*it)->get_transform()->apply_physics(delta_time);
}


ObjectPool<Asteroid> Asteroid::asteroids_pool_ = ObjectPool<Asteroid>(
	std::bind(&Asteroid::create_asteroid_func),
	std::bind(&Asteroid::on_get_asteroid_func, std::placeholders::_1),
	std::bind(&Asteroid::on_release_asteroid_func, std::placeholders::_1),
	nullptr);
std::shared_ptr<Asteroid> Asteroid::create_asteroid_func()
{
	return std::make_shared<Asteroid>(glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f), 0);
}
void Asteroid::on_get_asteroid_func(std::shared_ptr<Asteroid> instance)
{
	instance->set_is_active(true);
	all_active_asteroids_.insert(instance);
}
void Asteroid::on_release_asteroid_func(std::shared_ptr<Asteroid> instance)
{
	instance->set_is_active(false);

	all_active_asteroids_.erase(instance);
}