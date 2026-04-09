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


extern "C" PHYSICS_API void update_physics(Transform* const transform, float delta_time)
{
	const float kBasicallyZeroSqrLength = 0.01f * 0.01f;

	// ----- Velocity -----
	// Drag.
	float drag_factor = 1.0f - (0.1f * delta_time);
	transform->set_velocity(transform->get_velocity() * drag_factor);
	//if (glm::length2(transform->get_velocity()) <= kBasicallyZeroSqrLength)
	//	transform->set_velocity(glm::vec3(0.0f));

	// Clamp to maximum.
	const float kMaxVelocityMagnitude = 30.0f;
	const float kSqrMaxVelocityMagnitude = kMaxVelocityMagnitude * kMaxVelocityMagnitude;
	if (glm::length2(transform->get_velocity()) >= kSqrMaxVelocityMagnitude)
		transform->set_velocity(glm::normalize(transform->get_velocity()) * kMaxVelocityMagnitude);


	// ----- Angular Velocity -----
	// Drag.
	float angular_drag_factor = 1.0f - (2.0f * delta_time);
	transform->set_angular_velocity(transform->get_angular_velocity() * angular_drag_factor);
	//if (glm::length2(transform->get_angular_velocity()) <= kBasicallyZeroSqrLength)
	//	transform->set_angular_velocity(glm::vec3(0.0f));

	// Clamp to maximum.
	const float kMaxAngularVelocityMagnitude = glm::radians(360.0f);
	const float kSqrMaxAngularVelocityMagnitude = kMaxAngularVelocityMagnitude * kMaxAngularVelocityMagnitude;
	if (glm::length2(transform->get_angular_velocity()) >= kSqrMaxAngularVelocityMagnitude)
	{
		transform->set_angular_velocity(glm::normalize(transform->get_angular_velocity()) * kMaxAngularVelocityMagnitude);
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
extern "C" PHYSICS_API void sweep_and_prune(std::vector<Edge*>& edges, std::set<std::pair<Collider*, Collider*>>& overlapping)
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