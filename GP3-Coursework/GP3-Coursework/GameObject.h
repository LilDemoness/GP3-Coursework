#pragma once

#include <string>
#include "Mesh.h"
#include "Transform.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "Camera.h"

// A Temporary GameObject class which will be replaced at some point for code efficiency.
class GameObject
{
public:
	GameObject(const std::string& mesh_file_name, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
	GameObject(Mesh* mesh, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
	~GameObject();


	void Draw(const Camera& camera);
	void Draw(std::shared_ptr<Shader> shader, const Camera& camera);

	Transform* get_transform();	// Returns a pointer so that we can edit the original through the returned reference.
	Mesh* get_mesh() const;

private:
	Transform transform_;	// Can change to a pointer if we need 'get_transform()' to be const.
	Mesh* mesh_;	// SharedPtr for when meshes can be shared?
};