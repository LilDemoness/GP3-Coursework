#pragma once


#include <glm\glm.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Transform.h"
#include "Collision.h"
#include "Event.h"
#include "Mesh.h"


class Collider
{
public:
	struct Edge
	{
		Edge(Collider* collider, bool is_left) :
			collider(collider),
			is_left(is_left)
		{
			all_edges.emplace(all_edges.end(), this);
		}
		~Edge()
		{
			for (unsigned int i = 0; i < all_edges.size(); ++i)
				if (all_edges[i] == this)
					all_edges.erase(all_edges.begin() + i);
		}

		inline const float get_x_position() const
		{
			float x_pos = collider->get_transform()->get_pos().x + (is_left ? -collider->get_aabb_half_extents().x : collider->get_aabb_half_extents().x);
			return x_pos;
		}


		Collider* collider;
		bool is_left;

		static std::vector<Collider::Edge*> all_edges;
	};

	enum CollisionTag
	{
		kBlackHole,
		kAsteroid,

		kEnemy,
		kEnemyProjectile,

		kPlayer,
		kPlayerProjectile,

		kUndefined,
	};


	Collider(std::shared_ptr<Transform> transform, const Mesh* mesh, Collider::CollisionTag tag) :
		tag_(tag),
		enabled_(true),
		transform_(transform),
		edges_{ new Collider::Edge(this, true), new Collider::Edge(this, false) }
	{
		update_from_mesh(mesh);
		update_bounds();
		transform_->on_rotation_changed.subscribe(std::bind(&Collider::mark_bounds_as_dirty, this));
	}
	~Collider()
	{
		delete edges_[0];
		delete edges_[1];
		transform_->on_rotation_changed.unsubscribe(std::bind(&Collider::mark_bounds_as_dirty, this));
	}


	inline void set_enabled(const bool& new_value) { enabled_ = new_value; }
	const bool get_enabled() const { return enabled_; }


	const float get_radius() const { return radius_; }


	void update_from_mesh(const Mesh* const mesh)
	{
		// Use the vertices of the mesh to get the min & max bounds.
		const std::vector<glm::vec3>& vertex_positions = mesh->get_vertex_positions();
		if (vertex_positions.size() <= 0)
		{
			std::cerr << "Failed to update Collider bounds from mesh" << std::endl;
			return;
		}

		glm::vec3 min = vertex_positions[0];
		glm::vec3 max = vertex_positions[0];
		for (unsigned int i = 1; i < vertex_positions.size(); ++i)
		{
			// Min checks.
			if (vertex_positions[i].x < min.x) { min.x = vertex_positions[i].x; }
			if (vertex_positions[i].y < min.y) { min.y = vertex_positions[i].y; }
			if (vertex_positions[i].z < min.z) { min.z = vertex_positions[i].z; }

			// Max checks.
			if (vertex_positions[i].x > max.x) { max.x = vertex_positions[i].x; }
			if (vertex_positions[i].y > max.y) { max.y = vertex_positions[i].y; }
			if (vertex_positions[i].z > max.z) { max.z = vertex_positions[i].z; }
		}

		half_extents_ = (max - min) / 2.0f;
		radius_ = glm::length(half_extents_);
	}



	// Updates this collider's AABB bounding box's half extents.
	void update_bounds()
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
		bounds_dirty_ = false;
	}
	// Returns the collider's AABB bounding box's half extents.
	const glm::vec3 get_aabb_half_extents() 
	{
		if (bounds_dirty_)
			update_bounds();

		return aabb_half_extents_ ;
	}

	const Transform* get_transform() const { return transform_.get(); }


	static const bool is_valid_collision(Collider* a, Collider* b) { return test_collision_pair(a->tag_, b->tag_); } //{ return test_collision_pair(a->tag_, b->tag_) && test_collision_pair(b->tag_, a->tag_); }
	static const bool test_collision_pair(CollisionTag a, CollisionTag b)
	{
		if (a == CollisionTag::kUndefined || b == CollisionTag::kUndefined)
			return false;	// Undefined Collision Tags ignore collisions.

		switch (a)
		{
		case CollisionTag::kAsteroid: return b != kAsteroid;
		case CollisionTag::kPlayer: return b != kPlayerProjectile;
		case CollisionTag::kPlayerProjectile: return b != kPlayer;

		default: return true;
		};
	}

	const CollisionTag get_collision_tag() const { return tag_; }


	// <Collider* Self, Collider* Other>
	Event<Collider*, Collider*> on_collision_event;

private:
	void mark_bounds_as_dirty() { bounds_dirty_ = true; }
	bool enabled_;
	CollisionTag tag_;


	float radius_;
	glm::vec3 half_extents_;

	bool bounds_dirty_;
	glm::vec3 aabb_half_extents_;

	std::shared_ptr<Transform> transform_;
	Edge* edges_[2];
};