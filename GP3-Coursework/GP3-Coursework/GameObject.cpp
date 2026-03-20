#pragma once

#include "GameObject.h"

GameObject::GameObject(const std::string& mesh_file_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const float collision_radius) :
	mesh_(new Mesh(mesh_file_name)),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius))
{}
GameObject::GameObject(Mesh* mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const float collision_radius) :
	mesh_(mesh),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	collider_(std::make_shared<Collider>(transform_, collision_radius))
{}
GameObject::~GameObject()
{

}


void GameObject::draw(const Camera& camera)
{
	draw(camera, ShaderManager::get_instance().get_active_shader());
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