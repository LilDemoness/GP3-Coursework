#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Transform.h"
#include <glm/glm.hpp>

extern "C" PHYSICS_API void hello_world();


extern "C" PHYSICS_API void add_thrust(Transform* transform, float thrust_amount);

extern "C" PHYSICS_API void add_pitch(Transform* transform, float pitch_radians);
extern "C" PHYSICS_API void add_yaw(Transform* transform, float yaw_radians);
extern "C" PHYSICS_API void add_roll(Transform* transform, float roll_radians);


extern "C" PHYSICS_API void update_physics(Transform* transform, float delta_time);