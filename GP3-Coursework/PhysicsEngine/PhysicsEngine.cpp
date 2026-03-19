#pragma once

#include "pch.h"
#include "PhysicsEngine.h"
#include <iostream>

extern "C" PHYSICS_API void hello_world()
{
	std::cout << "Hello world from Physics Engine DLL" << std::endl;
}


extern "C" PHYSICS_API void add_thrust(Transform* transform, float thrust_amount)
{
	transform->add_force(transform->get_forward() * thrust_amount);

}

extern "C" PHYSICS_API void add_pitch(Transform* transform, float pitch_radians)
{
	transform->add_torque(glm::vec3(pitch_radians, 0.0f, 0.0f));
}
extern "C" PHYSICS_API void add_yaw(Transform* transform, float yaw_radians)
{
	transform->add_torque(glm::vec3(0.0f, yaw_radians, 0.0f));
}
extern "C" PHYSICS_API void add_roll(Transform* transform, float roll_radians)
{
	transform->add_torque(glm::vec3(0.0f, 0.0f, roll_radians));
}


extern "C" PHYSICS_API void update_physics(Transform* transform, float delta_time)
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