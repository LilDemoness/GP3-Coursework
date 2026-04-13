#pragma once

#include "GameObject.h"

std::unordered_set<GameObject*> GameObject::all_gameobjects_ = std::unordered_set<GameObject*>();
GameObject::GameObject(const std::string& mesh_file_name, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float collision_radius, bool add_to_all_objects) :
	mesh_(new Mesh(mesh_file_name)),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius, tag)),
	shader_tag_(),
	is_active_(true)
{
	if (add_to_all_objects)
		all_gameobjects_.insert(this);
}
GameObject::GameObject(Mesh* mesh, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float collision_radius, bool add_to_all_objects) :
	mesh_(mesh),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius, tag)),
	shader_tag_(),
	is_active_(true)
{
	if (add_to_all_objects)
		all_gameobjects_.insert(this);
}
GameObject::~GameObject()
{
	all_gameobjects_.erase(this);
}


void GameObject::draw_all(const Camera& camera)
{
	for (GameObject* const gameObject : all_gameobjects_)
	{
		if (gameObject->get_is_active())
			gameObject->draw(camera);
	}
}
void GameObject::draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader)
{
	for (GameObject* const gameObject : all_gameobjects_)
	{
		if (gameObject->get_is_active())
			gameObject->draw(camera, override_shader);
	}
}
void GameObject::draw(const Camera& camera)
{
	std::shared_ptr<Shader> shader = shader_tag_.empty() ? ShaderManager::get_instance().get_active_shader() : ShaderManager::get_instance().get_shader(shader_tag_);
	draw(camera, shader);
}
void GameObject::draw(const Camera& camera, std::shared_ptr<Shader> shader)
{
	shader->bind();
	shader->update_matrices_ubo(transform_.get(), camera);
	mesh_->draw();
}


Transform* GameObject::get_transform() const { return transform_.get(); }
Collider* GameObject::get_collider() const { return collider_.get(); }
Mesh* GameObject::get_mesh() const { return mesh_; }