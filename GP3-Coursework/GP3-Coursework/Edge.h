#pragma once

#include "Collider.h";

struct Edge
{
	Collider* collider;
	bool is_left;

	Edge(Collider* collider, bool is_left)
	{
		this->collider = collider;
		this->is_left = is_left;
	}

	float get_x_position()
	{
		if (is_left)
			return collider->get_transform()->get_pos().x - collider->get_aabb_half_extents().x;
		else
			return collider->get_transform()->get_pos().x + collider->get_aabb_half_extents().x;
	}
};