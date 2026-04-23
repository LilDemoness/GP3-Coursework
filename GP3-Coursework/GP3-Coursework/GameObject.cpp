#pragma once

#include "GameObject.h"

GameObject::GameObject(const std::string& mesh_file_name, const std::string& diffuse_texture_file_name, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, bool add_to_all_objects) :
	mesh_(Mesh::create_mesh(mesh_file_name)),
	texture_(Texture::create_texture(diffuse_texture_file_name)),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	shader_tag_(),
	is_active_(true)
{
	collider_ = std::make_shared<Collider>(transform_, mesh_.get(), tag);
}
GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> diffuse_texture, Collider::CollisionTag tag, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, bool add_to_all_objects) :
	mesh_(mesh),
	texture_(diffuse_texture),
	transform_(std::make_shared<Transform>(position, rotation, scale)),
	shader_tag_(),
	is_active_(true)
{
	collider_ = std::make_shared<Collider>(transform_, mesh_.get(), tag);

}
GameObject::~GameObject()
{
	mesh_->return_instance();
}


void GameObject::draw(const Camera& camera)
{
	std::shared_ptr<Shader> shader = shader_tag_.empty() ? ShaderManager::get_active_shader() : ShaderManager::get_shader(shader_tag_);
	draw(camera, shader);
}
void GameObject::draw(const Camera& camera, std::shared_ptr<Shader> shader)
{
	shader->bind();
	shader->update_matrices_ubo(camera);

	texture_->bind(0);

	mesh_->set_instance_matrix(0, transform_->get_model());
	mesh_->draw();
}


Transform* GameObject::get_transform() const { return transform_.get(); }
Collider* GameObject::get_collider() const { return collider_.get(); }
std::shared_ptr<Mesh> GameObject::get_mesh() const { return mesh_; }
std::shared_ptr<Texture> GameObject::get_texture() const { return texture_; }