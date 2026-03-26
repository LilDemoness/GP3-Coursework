#pragma once

#include "Collider.h"

std::vector<Edge*> Collider::edges_;

void Collider::RegisterCollider(Collider* collider)
{
	edges_.insert(edges_.begin(), new Edge(collider, false));
	edges_.insert(edges_.begin(), new Edge(collider, true));
}
void Collider::DeregisterCollider(Collider* collider)
{
	for (int i = 0; i < edges_.size(); ++i)
	{
		if (edges_[i]->collider == collider)
		{
			edges_.erase(edges_.begin() + i);
			--i;
		}
	}
}



const float Collider::get_radius() const { return radius_; }
// Updates this collider's AABB bounding box's half extents.
void Collider::update_bounds()
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
}
// Returns the collider's AABB bounding box's half extents.
const glm::vec3 Collider::get_aabb_half_extents() const { return aabb_half_extents_; }

const Transform* Collider::get_transform() const { return transform_.get(); }