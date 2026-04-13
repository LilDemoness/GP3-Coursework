#pragma once
#include "Projectile.h"


float Projectile::kSpeed = 15.0f;
Projectile::Projectile() : GameObject(get_projectile_mesh(), Collider::CollisionTag::kPlayerProjectile, glm::vec3(0.0f), glm::quat(), glm::vec3(0.2f), true)
{
	collider_->on_collision_event.subscribe(std::bind(&Projectile::on_collision_enter, this, std::placeholders::_1, std::placeholders::_2));
}
Projectile::~Projectile()
{
	collider_->on_collision_event.unsubscribe(std::bind(&Projectile::on_collision_enter, this, std::placeholders::_1, std::placeholders::_2));
}
void Projectile::dispose_all()
{
	active_projectiles_.clear();
	projectile_pool_.clear();
}


void Projectile::on_collision_enter(Collider* self, Collider* other)
{
	for (auto it = active_projectiles_.begin(); it != active_projectiles_.end();)
	{
		auto current = it++;
		if ((current->get()) == this)
		{
			projectile_pool_.release(*current);
		}
	}
}


void Projectile::spawn_projectile(glm::vec3 pos, glm::quat rot, glm::vec3 player_velocity)
{
	std::shared_ptr<Projectile> projectile_instance = projectile_pool_.get();

	// Setup the projectile.
	Transform* projectile_transform = projectile_instance->get_transform();
	projectile_transform->set_pos(pos);
	projectile_transform->set_rot(rot);

	projectile_transform->set_velocity(player_velocity + projectile_transform->get_forward() * kSpeed);

	// Disable collisions with the player until they leave them.
}


Mesh* Projectile::projectile_mesh_ = nullptr;
Mesh* Projectile::get_projectile_mesh()
{
	if (projectile_mesh_ == nullptr)
		projectile_mesh_ = new Mesh(PROJECTILE_MODEL_PATH);

	return projectile_mesh_;
}


std::unordered_set<std::shared_ptr<Projectile>> Projectile::active_projectiles_ = std::unordered_set<std::shared_ptr<Projectile>>();
void Projectile::update_projectiles(float delta_time)
{
	for (auto it = active_projectiles_.begin(); it != active_projectiles_.end(); ++it)
		(*it)->get_transform()->apply_physics(delta_time);
}


void Projectile::draw_all(const Camera& camera)
{
	for (auto it = active_projectiles_.begin(); it != active_projectiles_.end(); ++it)
	{
		if ((*it)->get_is_active())
		{
			(*it)->draw(camera);
		}
	}
}
void Projectile::draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader)
{
	for (auto it = active_projectiles_.begin(); it != active_projectiles_.end(); ++it)
	{
		if ((*it)->get_is_active())
		{
			(*it)->draw(camera, override_shader);
		}
	}
}


ObjectPool<Projectile> Projectile::projectile_pool_ = ObjectPool<Projectile>(
	std::bind(&Projectile::create_projectile_func),
	std::bind(&Projectile::on_get_projectile_func, std::placeholders::_1),
	std::bind(&Projectile::on_release_projectile_func, std::placeholders::_1),
	nullptr);
std::shared_ptr<Projectile> Projectile::create_projectile_func()
{
	return std::make_shared<Projectile>();
}
void Projectile::on_get_projectile_func(std::shared_ptr<Projectile> instance)
{
	instance->set_is_active(true);
	active_projectiles_.insert(instance);
}
void Projectile::on_release_projectile_func(std::shared_ptr<Projectile> instance)
{
	instance->set_is_active(false);
	active_projectiles_.erase(instance);
}