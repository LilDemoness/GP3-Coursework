#pragma once

#include "GameObject.h"
#include "ObjectPool.h"
#include <random>

#define ASTEROIDS_MODEL_PATH "..\\res\\cube1m.obj"

class Asteroid : public GameObject
{
public:
	Asteroid();
	Asteroid(glm::vec3 position, glm::quat rot, glm::vec3 scale, int remaining_splits);
	~Asteroid();

	static void create_initial_asteroids(int asteroids_count, int asteroid_splits, float world_bounds);
	void setup_asteroid(int remaining_splits, glm::vec3 position, glm::quat rot, float scale, glm::vec3 velocity, glm::vec3 angular_velocity);

	static std::unordered_set<std::shared_ptr<Asteroid>> all_asteroids_;
	void test_split(int split_iteration);

private:
	void split();

	int remaining_splits_;



	static Mesh* asteroids_mesh_;
	static Mesh* get_random_asteroid_mesh();

	static glm::vec3 get_random_direction_vector(std::mt19937 gen);
	static glm::quat get_random_direction(std::mt19937 gen);
	static float get_scale_for_remaining_splits(int remaining_splits);

	static ObjectPool<Asteroid> asteroids_pool_;
	static std::shared_ptr<Asteroid> create_asteroid_func();
	static void on_get_asteroid_func(std::shared_ptr<Asteroid> instance);
	static void on_release_asteroid_func(std::shared_ptr<Asteroid> instance);

};