#pragma once


#include <glm\glm.hpp>
#include <memory>

#include "Transform.h"

class Collider
{
public:
	Collider(std::shared_ptr<Transform> transform, float radius) :
		radius_(radius),
		half_extents_(glm::vec3(radius)),
		transform_(transform)
	{
		update_bounds();
		transform_->subscribe_to_rotation_change(std::bind(&Collider::mark_bounds_as_dirty, this));
	}
	~Collider()
	{
		transform_->unsubscribe_from_rotation_change(std::bind(&Collider::mark_bounds_as_dirty, this));
	}



	const float get_radius() const { return radius_; }
	// Updates this collider's AABB bounding box's half extents.
	void update_bounds()
	{
		glm::vec3 right = transform_->get_right() * half_extents_.x * transform_->get_scale().x;
		glm::vec3 up = transform_->get_up() * half_extents_.y * transform_->get_scale().x;
		glm::vec3 fwd = transform_->get_forward() * half_extents_.z * transform_->get_scale().x;

		glm::vec3 vertices[8]
		{
			fwd + up + right,	// pos_pos_pos.
			fwd + up - right,	// pos_pos_neg.
			fwd - up + right,	// pos_neg_pos.
			fwd - up - right,	// pos_meg_neg.

			-fwd + up + right,	// neg_pos_pos.
			-fwd + up - right,	// neg_pos_neg.
			-fwd - up + right,	// neg_neg_pos.
			-fwd - up - right	// neg_neg_neg.
		};

		float max_x = 0, max_y = 0, max_z = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (vertices[i].x > max_x)
				max_x = vertices[i].x;
			if (vertices[i].y > max_y)
				max_y = vertices[i].y;
			if (vertices[i].z > max_z)
				max_z = vertices[i].z;
		}

		aabb_half_extents_ = glm::vec3(max_x, max_y, max_z);
		bounds_dirty_ = false;
	}
	// Returns the collider's AABB bounding box's half extents.
	const glm::vec3 get_aabb_half_extents() 
	{
		if (bounds_dirty_)
			update_bounds();

		return aabb_half_extents_ ;
	}

	const Transform* get_transform() const { return transform_.get(); }

private:
	void mark_bounds_as_dirty() { bounds_dirty_ = true; }


	float radius_;
	glm::vec3 half_extents_;

	bool bounds_dirty_;
	glm::vec3 aabb_half_extents_;

	std::shared_ptr<Transform> transform_;
};