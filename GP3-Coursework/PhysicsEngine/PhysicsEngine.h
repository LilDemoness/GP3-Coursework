#pragma once

#ifdef PHYSICSENGINE_EXPORTS
#define PHYSICS_API __declspec(dllexport)
#else
#define PHYSICS_API __declspec(dllimport)
#endif

#include "Transform.h"
#include "Collider.h"
#include "Simplex.h"
#include <glm/glm.hpp>
#include <set>
#include <algorithm>

extern "C" PHYSICS_API void hello_world();


extern "C" PHYSICS_API void add_thrust(Transform* const transform, float thrust_amount);

extern "C" PHYSICS_API void add_pitch(Transform* const transform, float pitch_radians);
extern "C" PHYSICS_API void add_yaw(Transform* const transform, float yaw_radians);
extern "C" PHYSICS_API void add_roll(Transform* const transform, float roll_radians);


extern "C" PHYSICS_API void update_physics(Transform* const transform, float gravity_multiplier, bool apply_drag, float delta_time);
extern "C" PHYSICS_API void apply_physics(Transform* const transform, float world_radius, float delta_time);
extern "C" PHYSICS_API glm::vec3 loop_position_within_bounds(const glm::vec3& world_pos, const float world_radius);


extern "C" PHYSICS_API bool check_collisions(Collider* const a, Collider* const b);

// Determines if a collision is occuring using either a radius or AABB check depending on the settings of the passed colliders.
// Radius check takes priority.
extern "C" PHYSICS_API bool check_collisions_broad(Collider* const a, Collider* const b);
// Determines if a collision is occuring using the radius of two colliders.
extern "C" PHYSICS_API bool check_collisions_radius(Collider* const a, Collider* const b);
// Determines if a collision is occuring using the Axis-Aligned Bounding Boxes of two colliders.
extern "C" PHYSICS_API bool check_collisions_aabb(Collider* const a, Collider* const b);


extern "C" PHYSICS_API bool check_collisions_narrow(Collider* const a, Collider* const b);
extern "C" PHYSICS_API bool next_simplex(Simplex& points, glm::vec3& direction);
extern "C" PHYSICS_API bool test_line(Simplex& points, glm::vec3& direction);
extern "C" PHYSICS_API bool test_triangle(Simplex& points, glm::vec3& direction);
extern "C" PHYSICS_API bool test_tetrahedron(Simplex& points, glm::vec3& direction);

extern "C" PHYSICS_API bool is_same_direction(const glm::vec3& direction_a, const glm::vec3& direction_b);



extern "C" PHYSICS_API void sweep_and_prune(std::vector<Collider::Edge*>& edges, std::set<std::pair<Collider*, Collider*>>& overlapping);