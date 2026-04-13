#pragma once

#include <string>
#include <unordered_set>
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
	GameObject(const std::string& mesh_file_name, Collider::CollisionTag tag, const glm::vec3& position = glm::vec3(0.0f), const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f), const float collision_radius = 0.5f, bool add_to_all_objects = false);
	GameObject(Mesh* mesh, Collider::CollisionTag tag, const glm::vec3& position = glm::vec3(0.0f), const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f), const float collision_radius = 0.5f, bool add_to_all_objects = false);
	~GameObject();


	static void draw_all(const Camera& camera);
	static void draw_all(const Camera& camera, std::shared_ptr<Shader> override_shader);
	void draw(const Camera& camera);
	void draw(const Camera& camera, std::shared_ptr<Shader> shader);

	Transform* get_transform() const;
	Collider* get_collider() const;
	Mesh* get_mesh() const;

	inline void set_shader_tag(const std::string& new_value) { shader_tag_ = new_value; }

	inline const bool get_is_active() const { return is_active_; }
	inline void set_is_active(bool new_value)
	{
		is_active_ = new_value;
		collider_->set_enabled(new_value);
	}

protected:
	std::shared_ptr<Transform> transform_;
	std::shared_ptr<Collider> collider_;

private:
	Mesh* mesh_;	// SharedPtr for when meshes can be shared?
	bool is_active_;
	std::string shader_tag_;


	static std::unordered_set<GameObject*> all_gameobjects_;
};