#pragma once

#include <string>
#include "Mesh.h"
#include "Transform.h"
#include "Collider.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Camera.h"

// A Temporary GameObject class which will be replaced at some point for code efficiency.
class GameObject
{
public:
	GameObject(const std::string& mesh_file_name, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f), const float collision_radius = 0.5f);
	GameObject(Mesh* mesh, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f), const float collision_radius = 0.5f);
	~GameObject();


	void draw(const Camera& camera);
	void draw(const Camera& camera, std::shared_ptr<Shader> shader);

	Transform* get_transform() const;
	Collider* get_collider() const;
	Mesh* get_mesh() const;

private:
	std::shared_ptr<Transform> transform_;
	std::shared_ptr<Collider> collider_;
	Mesh* mesh_;	// SharedPtr for when meshes can be shared?
};