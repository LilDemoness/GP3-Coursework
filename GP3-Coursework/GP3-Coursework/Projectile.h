#pragma once

#include "GameObject.h"
#include "ObjectPool.h"

#define PROJECTILE_MODEL_PATH "..\\res\\cube1m.obj"

class Projectile : public GameObject
{
public:
	Projectile();
	~Projectile();
	static void dispose_all();

	// Delete Copy Constructor.
	Projectile(const Projectile& other) = delete;
	Projectile& operator= (const Projectile& other) = delete;

	void on_collision_enter(Collider* self, Collider* other);

	static void spawn_projectile(glm::vec3 position, glm::quat rotation, glm::vec3 player_velocity);
	static void update_projectiles(float delta_time);

	static void draw_all(const Camera& camera);
	static void draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader);

private:
	static float kSpeed;


	// Mesh.
	static Mesh* projectile_mesh_;
	static Mesh* get_projectile_mesh();

	static std::unordered_set<std::shared_ptr<Projectile>> active_projectiles_;

	// Object Pooling.
	static ObjectPool<Projectile> projectile_pool_;
	static std::shared_ptr<Projectile> create_projectile_func();
	static void on_get_projectile_func(std::shared_ptr<Projectile> instance);
	static void on_release_projectile_func(std::shared_ptr<Projectile> instance);
};