#pragma once

#include "GameObject.h"
#include "ObjectPool.h"
#include "Event.h"
#include <random>

#define ASTEROIDS_MODEL_PATH "..\\res\\Asteroid.obj"

class Asteroid : public GameObject
{
public:
	Asteroid();
	Asteroid(glm::vec3 position, glm::quat rot, glm::vec3 scale, int remaining_splits);
	~Asteroid();
	static void dispose_all();

	// Delete Copy Constructor.
	Asteroid(const Asteroid& other) = delete;
	Asteroid& operator= (const Asteroid& other) = delete;

	static void create_initial_asteroids(const int asteroids_count, const int asteroid_split_count, const float min_velocity, const float max_velocity, const float world_bounds);
	void setup_asteroid(int remaining_splits, glm::vec3 position, glm::quat rot, float scale, glm::vec3 velocity, glm::vec3 angular_velocity);


	static void update_all_asteroids(float delta_time);

	static void draw_all(const Camera& camera);
	static void draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader);
		
	static Event<int> on_any_asteroid_destroyed;
	static Event<> on_all_asteroids_destroyed;


	static void kill_all_asteroids();

private:
	void on_collision(Collider* self, Collider* other);
	void split();

	int remaining_splits_;


	// Mesh Instancing.
	static constexpr int kMaxAsteroids = 50;

	// Spawning.
	static glm::vec3 get_random_direction_vector(std::mt19937 gen);
	static glm::quat get_random_direction(std::mt19937 gen);
	static float get_scale_for_remaining_splits(int remaining_splits);

	static int get_score_for_size(int remaining_splits);

	static std::unordered_set<std::shared_ptr<Asteroid>> all_active_asteroids_;


	// Object Pooling.
	static ObjectPool<Asteroid> asteroids_pool_;
	static std::shared_ptr<Asteroid> create_asteroid_func();
	static void on_get_asteroid_func(std::shared_ptr<Asteroid> instance);
	static void on_release_asteroid_func(std::shared_ptr<Asteroid> instance);
};