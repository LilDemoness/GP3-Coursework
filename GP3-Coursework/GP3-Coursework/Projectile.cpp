#pragma once
#include "Projectile.h"


float Projectile::kSpeed = 15.0f;
Projectile::Projectile() : GameObject(Mesh::create_mesh(PROJECTILE_MODEL_PATH), Texture::create_texture(PROJECTILE_TEXTURE_PATH), Collider::CollisionTag::kPlayerProjectile, glm::vec3(0.0f), glm::quat(), glm::vec3(0.2f), true)
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
		std::shared_ptr<Projectile> instance = *(it++);
		if (instance.get() == this)
		{
			projectile_pool_.release(instance);

			// Ensure that the projectile is removed they are occasionally missed.
			instance->set_is_active(false);
			active_projectiles_.erase(instance);

			return;
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


std::unordered_set<std::shared_ptr<Projectile>> Projectile::active_projectiles_ = std::unordered_set<std::shared_ptr<Projectile>>();


void Projectile::draw_all(const Camera& camera)
{
	draw_all(camera, ShaderManager::get_active_shader());
}
void Projectile::draw_all(const Camera& camera, std::shared_ptr<Shader> shader)
{
	if (active_projectiles_.empty())
		return;

	shader->bind();
	shader->update_matrices_ubo(camera);

	std::shared_ptr<Mesh> projectile_mesh = active_projectiles_.begin()->get()->get_mesh();
	active_projectiles_.begin()->get()->get_texture()->bind(0);

	// Bind all model matrices.
	int i = 0;
	projectile_mesh->bind_vao();
	for (const std::shared_ptr<Projectile> projectile : active_projectiles_)
	{
		if (projectile->get_is_active() && projectile->get_is_shown())
		{
			projectile_mesh->set_instance_matrix(i, projectile->get_transform()->get_model());
			i++;
		}
	}

	// Render.
	projectile_mesh->draw_instanced(active_projectiles_.size());
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