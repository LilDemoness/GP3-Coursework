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


void GameObject::Draw(const Camera& camera)
{
	Draw(ShaderManager::get_instance().GetActiveShader(), camera);
}
void GameObject::Draw(std::shared_ptr<Shader> shader, const Camera& camera)
{
	shader->Bind();
	shader->UpdateMatricesUBO(transform_, camera);
	mesh_->Draw();
}


Transform* GameObject::get_transform() { return &transform_; }
Mesh* GameObject::get_mesh() const { return mesh_; }