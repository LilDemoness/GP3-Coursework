#pragma once
#include "Asteroid.h"


Asteroid::Asteroid()
	: GameObject(get_random_asteroid_mesh(), Collider::CollisionTag::kAsteroid, glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f), 0.5f, true),
	remaining_splits_(0)
{
	collider_->on_collision_event.subscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));
}
Asteroid::Asteroid(glm::vec3 position, glm::quat rot, glm::vec3 scale, int remaining_splits)
	: GameObject(get_random_asteroid_mesh(), Collider::CollisionTag::kAsteroid, position, rot, scale, 0.5f, true),
	remaining_splits_(remaining_splits)
{
	collider_->on_collision_event.subscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));
}
Asteroid::~Asteroid()
{
	collider_->on_collision_event.unsubscribe(std::bind(&Asteroid::on_collision, this, std::placeholders::_1, std::placeholders::_2));
}


void Asteroid::create_initial_asteroids(int asteroids_count, int asteroid_split_count, float world_bounds)
{
	const glm::vec3 kForward = glm::vec3(0.0f, 1.0f, 0.0f);

	// Setup randomness.
	const float kFloatResolution = 10.0f;
	std::random_device rd;
	std::mt19937 gen(rd());

	const float kMinVelocity = 0.2f;
	const float kMaxVelocity = 3.0f;
	std::uniform_int_distribution<> random_velocity_magnitude(kMinVelocity * kFloatResolution, kMaxVelocity * kFloatResolution);


	for (int i = 0; i < asteroids_count; ++i)
	{
		std::shared_ptr<Asteroid> asteroid = asteroids_pool_.get();

		glm::vec3 position = world_bounds * get_random_direction_vector(gen);
		glm::quat rotation = get_random_direction(gen);
		float scale = get_scale_for_remaining_splits(asteroid_split_count);

		//glm::vec3 velocity = (random_velocity_magnitude(gen) / (float)kFloatResolution) * get_random_direction_vector(gen);
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



void Asteroid::on_collision(Collider* self, Collider* other)
{
	// Check if 'other' is a projectile.
	if (true)
	{
		split();
	}
}
void Asteroid::split()
{
	if (remaining_splits_ <= 0)
	{
		for (auto asteroid : all_asteroids_)
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


Mesh* Asteroid::asteroids_mesh_ = nullptr;
Mesh* Asteroid::get_random_asteroid_mesh()
{
	if (asteroids_mesh_ == nullptr)
	{
		asteroids_mesh_ = new Mesh(ASTEROIDS_MODEL_PATH);
	}

	return asteroids_mesh_;
}


float Asteroid::get_scale_for_remaining_splits(int remaining_splits)
{
	/*
	// Increases linearly.
	const float kScaleIncrement = 0.5f;
	const float kBaseScale = 0.5f;
	return (remaining_splits * kScaleIncrement) + kBaseScale;*/

	// Doubles each time.
	const float kBaseScale = 0.25f;
	return std::powf(2, remaining_splits) * kBaseScale;
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
	all_asteroids_.insert(instance);
}
void Asteroid::on_release_asteroid_func(std::shared_ptr<Asteroid> instance)
{
	instance->set_is_active(false);

	all_asteroids_.erase(instance);
}

std::unordered_set<std::shared_ptr<Asteroid>> Asteroid::all_asteroids_ = std::unordered_set<std::shared_ptr<Asteroid>>();