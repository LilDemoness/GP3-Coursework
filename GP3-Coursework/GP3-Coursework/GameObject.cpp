#pragma once

#include "GameObject.h"

GameObject::GameObject(const std::string& mesh_file_name, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	: mesh_(new Mesh(mesh_file_name)),
      transform_(position, rotation, scale)
{}
GameObject::GameObject(Mesh* mesh, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	: mesh_(mesh),
	  transform_(position, rotation, scale)
{}
GameObject::~GameObject()
{

}


void GameObject::draw(const Camera& camera)
{
	draw(ShaderManager::get_instance().get_active_shader(), camera);
}
void GameObject::draw(std::shared_ptr<Shader> shader, const Camera& camera)
{
	shader->bind();
	shader->update_matrices_ubo(transform_, camera);
	mesh_->draw();
}


Transform* GameObject::get_transform() { return &transform_; }
Mesh* GameObject::get_mesh() const { return mesh_; }