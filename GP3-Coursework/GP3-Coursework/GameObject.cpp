#pragma once

#include "GameObject.h"

Event<GameObject*> GameObject::on_gameobject_created;
Event<GameObject*> GameObject::on_gameobject_destroyed;
GameObject::GameObject(const std::string& mesh_file_name, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float collision_radius, bool add_to_all_objects) :
	mesh_(new Mesh(mesh_file_name)),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius, tag)),
	shader_tag_(),
	is_active_(true)
{

}
GameObject::GameObject(Mesh* mesh, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, const float collision_radius, bool add_to_all_objects) :
	mesh_(mesh),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius, tag)),
	shader_tag_(),
	is_active_(true)
{

}
GameObject::~GameObject()
{}


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