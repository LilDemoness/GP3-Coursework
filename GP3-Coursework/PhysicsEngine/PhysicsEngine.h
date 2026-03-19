#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Transform.h"
#include <glm/glm.hpp>

extern "C" PHYSICS_API void hello_world();

extern "C" PHYSICS_API void set_forward_direction(Transform* transform, glm::vec3 forward);
extern "C" PHYSICS_API void apply_thrust(Transform* transform, float thrust_amount);
extern "C" PHYSICS_API void update_physics(Transform* transform, float delta_time);