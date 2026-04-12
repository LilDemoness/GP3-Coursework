#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Transform.h"
#include "Collider.h"
#include <glm/glm.hpp>
#include <set>
#include <algorithm>

extern "C" PHYSICS_API void hello_world();


extern "C" PHYSICS_API void add_thrust(Transform* const transform, float thrust_amount);

extern "C" PHYSICS_API void add_pitch(Transform* const transform, float pitch_radians);
extern "C" PHYSICS_API void add_yaw(Transform* const transform, float yaw_radians);
extern "C" PHYSICS_API void add_roll(Transform* const  transform, float roll_radians);


extern "C" PHYSICS_API void update_physics(Transform* const  transform, float delta_time);


// Determines if a collision is occuring using the radius of two colliders.
extern "C" PHYSICS_API bool check_collisions_radius(Collider* const a, Collider* const b);
// Determines if a collision is occuring using the Axis-Aligned Bounding Boxes of two colliders.
extern "C" PHYSICS_API bool check_collisions_aabb(Collider* const a, Collider* const b);

extern "C" PHYSICS_API void sweep_and_prune(std::vector<Collider::Edge*>& edges, std::set<std::pair<Collider*, Collider*>>& overlapping);