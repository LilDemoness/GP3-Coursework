#pragma once

#include "GameObject.h"
#include "ObjectPool.h"

#define PROJECTILE_MODEL_PATH "..\\res\\cube1m.obj"
#define PROJECTILE_TEXTURE_PATH "..\\res\\BlackPixel.jpg"

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
	static std::unordered_set<std::shared_ptr<Projectile>> active_projectiles_;

	static void draw_all(const Camera& camera);
	static void draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader);

private:
	static float kSpeed;


	// Object Pooling.
	static ObjectPool<Projectile> projectile_pool_;
	static std::shared_ptr<Projectile> create_projectile_func();
	static void on_get_projectile_func(std::shared_ptr<Projectile> instance);
	static void on_release_projectile_func(std::shared_ptr<Projectile> instance);
};