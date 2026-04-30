#pragma once

#include "pch.h"
#include "PhysicsEngine.h"
#include <iostream>

extern "C" PHYSICS_API void hello_world()
{
	std::cout << "Hello world from Physics Engine DLL" << std::endl;
}


extern "C" PHYSICS_API void add_thrust(Transform* const transform, float thrust_amount)
{
	transform->add_force(transform->get_forward() * thrust_amount);

}

extern "C" PHYSICS_API void add_pitch(Transform* const transform, float pitch_radians)
{
	transform->add_torque(glm::vec3(pitch_radians, 0.0f, 0.0f));
}
extern "C" PHYSICS_API void add_yaw(Transform* const transform, float yaw_radians)
{
	transform->add_torque(glm::vec3(0.0f, yaw_radians, 0.0f));
}
extern "C" PHYSICS_API void add_roll(Transform* const transform, float roll_radians)
{
	transform->add_torque(glm::vec3(0.0f, 0.0f, roll_radians));
}


extern "C" PHYSICS_API void update_physics(Transform* const transform, float gravity_multiplier, bool apply_drag, float delta_time)
{
	// ----- Velocity Changes -----
	// Drag.
	if (apply_drag)
	{
		float drag_factor = 1.0f - (0.1f * delta_time);
		transform->set_velocity(transform->get_velocity() * drag_factor);
	}
	
	// Black Hole Gravity.
	glm::vec3 dir_to_centre = -transform->get_pos();
	float dst_to_centre = glm::length(dir_to_centre);
	if (dst_to_centre >= 0.1f)
	{
		const float kMaxGravityConstant = 1.0f;
		const float kMinGravityConstant = 0.15f;
		const float kMaxEffectRange = 15.0f;
		dir_to_centre /= dst_to_centre;
		float effect_range_percent = dst_to_centre / kMaxEffectRange;
		if (effect_range_percent < 0.0f) { effect_range_percent = 0.0f; } else if (effect_range_percent > 1.0f) { effect_range_percent = 1.0f; } // Clamp01.
		float gravity_force_scale = kMaxGravityConstant + effect_range_percent * (kMinGravityConstant - kMaxGravityConstant);	// Lerp.

		transform->add_force(dir_to_centre * gravity_force_scale * gravity_multiplier * delta_time);
	}

	// Clamp to maximum.
	const float kMaxVelocityMagnitude = 30.0f;
	const float kSqrMaxVelocityMagnitude = kMaxVelocityMagnitude * kMaxVelocityMagnitude;
	if (glm::length2(transform->get_velocity()) >= kSqrMaxVelocityMagnitude)
		transform->set_velocity(glm::normalize(transform->get_velocity()) * kMaxVelocityMagnitude);


	// ----- Angular Velocity -----
	// Drag.
	if (apply_drag)
	{
		float angular_drag_factor = 1.0f - (2.0f * delta_time);
		transform->set_angular_velocity(transform->get_angular_velocity() * angular_drag_factor);
	}

	// Clamp to maximum.
	const float kMaxAngularVelocityMagnitude = glm::radians(360.0f);
	const float kSqrMaxAngularVelocityMagnitude = kMaxAngularVelocityMagnitude * kMaxAngularVelocityMagnitude;
	if (glm::length2(transform->get_angular_velocity()) >= kSqrMaxAngularVelocityMagnitude)
	{
		transform->set_angular_velocity(glm::normalize(transform->get_angular_velocity()) * kMaxAngularVelocityMagnitude);
	}
}
extern "C" PHYSICS_API void apply_physics(Transform* const transform, float world_radius, float delta_time)
{
	// Position / Velocity.
	glm::vec3 new_position = transform->get_pos() + transform->get_velocity() * delta_time;
	if (transform->get_ignore_bounds() == false)
		new_position = loop_position_within_bounds(new_position, world_radius);

	transform->set_pos(new_position);


	// Rotation / Angular Velocity.
	if (transform->get_angular_velocity() != glm::vec3(0.0f))
		transform->rotate(transform->get_angular_velocity(), glm::length(transform->get_angular_velocity()) * delta_time);
}

extern "C" PHYSICS_API glm::vec3 loop_position_within_bounds(const glm::vec3& world_pos, const float world_radius)
{
	float sqrDistanceFromOrigin = (world_pos.x * world_pos.x) + (world_pos.y * world_pos.y) + (world_pos.z * world_pos.z);

	if (sqrDistanceFromOrigin <= (world_radius * world_radius))
		return world_pos;	// We are within the world radius.

	// We are outwith the world radius.
	// Loop our movement outside of the world radius, ensuring to account for if were > 2x the radius.

	glm::vec3 direction_from_origin = glm::normalize(world_pos);
	float distance_from_origin = std::sqrt(sqrDistanceFromOrigin);	// Alt: world_pos.length();

	// Calculate the fractional part of how far we're outside the radius (Accounts for distance > 2 x kWorldRadius).
	int distance_from_origin_int = (int)distance_from_origin;
	float outside_distance_remainder = (distance_from_origin_int % (int)world_radius) + (distance_from_origin - (float)distance_from_origin_int);


	// Move to re-enter from the other side of the radius.
	return (-direction_from_origin * world_radius) + (direction_from_origin * outside_distance_remainder);
}



extern "C" PHYSICS_API bool check_collisions(Collider* const a, Collider* const b)
{
	if (a->get_use_radius() || b->get_use_radius())
	{
		return check_collisions_radius(a, b);
	}
	else
	{
		return check_collisions_aabb(a, b);
	}
}

extern "C" PHYSICS_API bool check_collisions_radius(Collider* const a, Collider* const b)
{
	float sqrDistance = glm::distance2(a->get_transform()->get_pos(), b->get_transform()->get_pos());
	float twinRadii = a->get_radius() + b->get_radius();
	return sqrDistance < (twinRadii * twinRadii);
}
extern "C" PHYSICS_API bool check_collisions_aabb(Collider* const a, Collider* const b)
{
	glm::vec3 a_pos = a->get_transform()->get_pos();
	glm::vec3 a_half_extents = a->get_aabb_half_extents();

	glm::vec3 b_pos = b->get_transform()->get_pos();
	glm::vec3 b_half_extents = b->get_aabb_half_extents();

	return glm::abs(a_pos.x - b_pos.x) < (a_half_extents.x + b_half_extents.x)
		&& glm::abs(a_pos.y - b_pos.y) < (a_half_extents.y + b_half_extents.y)
		&& glm::abs(a_pos.z - b_pos.z) < (a_half_extents.z + b_half_extents.z);
}


// Adaptation Source Link: 'https://leanrada.com/notes/sweep-and-prune-2/'.
extern "C" PHYSICS_API void sweep_and_prune(std::vector<Collider::Edge*>& edges, std::set<std::pair<Collider*, Collider*>>& overlapping)
{
	for (int i = 1; i < edges.size(); ++i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			if (edges[j]->get_x_position() < edges[j + 1]->get_x_position())
				break;

			// Sort.
			std::iter_swap(edges.begin() + j, edges.begin() + j + 1);

			// ----- Collision Detection -----
			if (edges[j]->is_left && !edges[j + 1]->is_left)
			{
				// Now overlapping.
				std::pair<Collider*, Collider*> key = std::make_pair(edges[j]->collider, edges[j + 1]->collider);
				overlapping.insert(key);
			}
			else if (!edges[j]->is_left && edges[j + 1]->is_left)
			{
				// No longer overlapping.
				std::pair<Collider*, Collider*> key = std::make_pair(edges[j]->collider, edges[j + 1]->collider);
				overlapping.erase(key);
			}
		}
	}
}