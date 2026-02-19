#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Transform.h"

#define FIELD_OF_VIEW 70.0f

struct Camera
{
public:
	Camera(const glm::vec3& position, float field_of_view, float aspect, float near_clip, float far_clip)
		: transform_(new Transform(position, glm::vec3(0.0f), glm::vec3(1.0f))),
		projection_(glm::perspective(field_of_view, aspect, near_clip, far_clip)),
		near_clip_(near_clip),
		far_clip_(far_clip)
	{ }
	~Camera()
	{
		delete transform_;
	}

	Transform* get_transform() const { return transform_; }

	inline glm::mat4 get_view_projection() const
	{
		return get_projection() * get_view();
	}
	inline glm::mat4 get_projection() const { return projection_; }
	inline glm::mat4 get_view() const
	{
		return glm::lookAt(transform_->get_pos(), transform_->get_pos() + transform_->get_forward(), transform_->get_up());
	}


	inline float get_near_clip() { return near_clip_; }
	inline float get_far_clip() { return far_clip_; }


protected:
private:
	Camera() = delete;
	Camera(Camera& other) = delete;

	Transform* transform_;
	glm::mat4 projection_;

	float near_clip_, far_clip_;
};