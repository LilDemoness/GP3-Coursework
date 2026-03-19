#pragma once

#include "pch.h"
#include "PhysicsEngine.h"
#include <iostream>

extern "C" PHYSICS_API void hello_world()
{
	std::cout << "Hello world from Physics Engine DLL" << std::endl;
}

extern "C" PHYSICS_API void set_forward_direction(Transform* transform, glm::vec3 forward)
{
	transform->set_forward(forward);
}
extern "C" PHYSICS_API void apply_thrust(Transform* transform, float thrust_amount)
{
	transform->add_force(transform->get_forward() * thrust_amount);
}
extern "C" PHYSICS_API void update_physics(Transform* transform, float delta_time)
{
	// Note: Currently framerate-dependant.
	const float drag_factor = 0.99f;
	transform->set_velocity(transform->get_velocity() * drag_factor);
}