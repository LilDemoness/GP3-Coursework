#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#include "Event.h"

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(0.0f), const glm::quat& rot = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f)) :
		parent_(nullptr),
		children_(std::vector<Transform*>()),
		velocity_(glm::vec3(0.0f)),
		angular_velocity_(glm::vec3(0.0f)),

		local_pos_(pos),
		local_rot_(rot),
		local_scale_(scale)
	{}

	inline glm::mat4 get_model() const
	{
		glm::mat4 posMat = glm::translate(get_pos());
		glm::mat4 rotMat = glm::toMat4(glm::inverse(get_rot())); // Rotation quaternion needs to be inversed before converted otherwise rotations are inverted (Idk why, poor self implementation probably).
		glm::mat4 scaleMat = glm::scale(get_scale());

		return posMat * rotMat * scaleMat;
	}


	// ----- Events -----
	Event<> on_position_changed;
	Event<> on_rotation_changed;
	Event<> on_scale_changed;


	static void set_world_radius(float world_radius) { world_radius_ = world_radius; }


	// ----- Position -----
	inline glm::vec3 get_pos() const
	{
		if (has_parent())
			return pos_local_to_world(local_pos_);
		else
			return local_pos_;
	}
	inline void set_pos(const glm::vec3& new_pos)
	{
		// Set our position.
		if (has_parent())
			set_local_pos(pos_world_to_local(new_pos));
		else
			set_local_pos(new_pos);
	}
	inline glm::vec3 get_local_pos() const { return local_pos_; }
	inline void set_local_pos(const glm::vec3& new_local_pos)
	{
		if (local_pos_ == new_local_pos)
			return;

		local_pos_ = new_local_pos;
		if (!has_parent())
		{
			local_pos_ = pos_world_to_local(loop_position_within_bounds(pos_local_to_world(local_pos_)));
		}

		// Set our children's position.
		for (int i = 0; i < this->get_child_count(); ++i)
		{
			Transform* child = this->get_child(i);
			child->set_local_pos(child->get_local_pos());
		}

		// Notify listeners of the change.
		on_position_changed.invoke();
	}
	
	
	inline glm::vec3 pos_world_to_local(const glm::vec3& world_space) const
	{
		glm::vec3 local_space = world_space;
		if (has_parent())
		{
			local_space -= parent_->get_pos(); // Offset.
			local_space /= parent_->get_scale();	// Scale.
			local_space = parent_->get_rot() * local_space;	// Rotation.
		}

		return local_space;
	}
	inline glm::vec3 pos_local_to_world(const glm::vec3& local_space) const
	{
		glm::vec3 world_space = local_space;
		if (has_parent())
		{
			world_space = world_space * parent_->get_rot();	// Rotation.
			world_space *= parent_->get_scale();	// Scale.
			world_space += parent_->get_pos();	// Offset.
		}

		return world_space;
	}


	// ----- Rotation -----
	inline glm::quat get_rot() const
	{
		if (has_parent())
			return diff(parent_->get_rot(), local_rot_);
		else
			return local_rot_;
	}
	inline glm::vec3 get_euler_angles() const { return glm::eulerAngles(get_rot()); }
	inline void set_euler_angles(const float& pitch, const float& yaw, const float& roll) { this->set_rot(glm::quat(glm::vec3(pitch, yaw, roll))); }
	inline void set_euler_angles(const glm::vec3& new_rot) { this->set_rot(glm::quat(new_rot)); }
	inline void set_rot(const glm::quat& new_rot)
	{
		// Set our rotation through our local rotation.
		if (parent_ != nullptr)
			set_local_rot(diff(parent_->get_rot(), get_rot()));
		else
			set_local_rot(new_rot);
	}

	inline glm::quat get_local_rot() const { return local_rot_; }
	inline void set_local_rot(glm::quat new_local_rot)
	{
		if (local_rot_ == new_local_rot)
			return;
		local_rot_ = new_local_rot;

		// Update our children's rotation.
		for (int i = 0; i < this->get_child_count(); ++i)
		{
			Transform* child = this->get_child(i);

			// Update Child Position using their local position.
			// This automatically accounts for rotation.
			child->set_local_pos(child->get_local_pos());

			// Update Child Rotation using their local rotation (Still the value from before we rotated).
			glm::quat new_world_rot = add(this->get_rot(), child->get_local_rot());
			child->set_rot(new_world_rot);
		}

		// Notify listeners of the change.
		on_rotation_changed.invoke();
	}


	// ----- Scale -----
	inline glm::vec3 get_scale() const
	{
		if (has_parent())
			return parent_->get_scale() * local_scale_;
		else
			return local_scale_;
	}
	inline void set_scale(glm::vec3 new_scale)
	{
		// Set our scale.
		if (has_parent())
			set_local_scale(get_scale() / parent_->get_scale());
		else
			set_local_scale(new_scale);
	}

	inline glm::vec3 get_local_scale() const { return local_scale_; }
	inline void set_local_scale(glm::vec3 new_local_scale)
	{
		if (local_scale_ == new_local_scale)
			return;
		local_scale_ = new_local_scale; 

		// Update our children's scale & relative positions.
		for (int i = 0; i < this->get_child_count(); ++i)
		{
			Transform* child = this->get_child(i);

			// Update Child Position using their local position.
			// This automatically accounts for scale.
			child->set_local_pos(child->get_local_pos());


			// Update child scale using their local scale (Still the value from before we rotated).
			glm::vec3 new_world_scale = this->get_scale() / child->get_local_scale();
			child->set_scale(new_world_scale);
		}

		// Notify listeners of the change.
		on_scale_changed.invoke();
	}


	// ----- Orientation -----
	inline glm::vec3 get_forward() const { return glm::normalize(kWorldForward * this->get_rot()); }
	inline glm::vec3 get_up() const { return glm::normalize(kWorldUp * this->get_rot()); }
	inline glm::vec3 get_right() const { return glm::normalize(kWorldRight * this->get_rot()); }

	inline void set_forward(const glm::vec3 forward)
	{
		//set_rot(glm::quat(forward, get_up()));
		set_rot(glm::quat(forward, kWorldUp));
	}



	enum RotationSpace
	{
		kLocalSpace,
		kWorldSpace
	};

	void rotate(glm::vec3 axis, float angle, RotationSpace rotationSpace = RotationSpace::kLocalSpace)
	{
		if (rotationSpace == RotationSpace::kWorldSpace)
		{
			// World Space Rotation.
			// Transform the desired axis of rotation based on our current rotation.
			axis = get_rot() * axis;
		}

		// Calculate and set our new rotation (Second Rot * First Rot).
		glm::quat orientation_quaternion = glm::angleAxis(angle, glm::normalize(axis));
		rotate(orientation_quaternion);
	}
	void rotate(glm::quat rotation)
	{
		this->set_rot(add(get_rot(), rotation));
	}
	void rotate_around_point(glm::vec3 point, glm::vec3 rotation_axis, float angle)
	{
		// rotate our position around the specified point.
		glm::quat position_orientation_quaternion = glm::angleAxis(angle, glm::normalize(rotation_axis));
		glm::vec3 rotated_point = point + (position_orientation_quaternion * (this->get_pos() - point));
		this->set_pos(rotated_point);

		// Transform the desired axis of rotation based on our current rotation.
		rotation_axis = get_rot() * rotation_axis;

		// Apply our rotation.
		this->set_rot(add(get_rot(), glm::angleAxis(-angle, glm::normalize(rotation_axis))));
	}
	void rotate_around_point(glm::vec3 point, glm::quat rotation)
	{
		// rotate our position around the specified point.
		glm::vec3 rotated_point = point + ((this->get_pos() - point) * rotation);
		this->set_pos(rotated_point);

		// Apply our rotation.
		this->set_rot(add(get_rot(), rotation));
	}


	// Velocity.
	inline glm::vec3 get_velocity() const { return this->velocity_; }
	inline void set_velocity(glm::vec3 new_velocity) { this->velocity_ = new_velocity; }

	inline glm::vec3 get_angular_velocity() const { return this->angular_velocity_; }
	inline void set_angular_velocity(glm::vec3 new_angular_velocity)
	{
		//const kInvalidVelocity = glm::vec3(float::NAN)
		if (!isnan(new_angular_velocity.x))
			this->angular_velocity_ = new_angular_velocity;
	}

	inline void add_force(glm::vec3 force) { this->velocity_ += force; }
	inline void add_torque(glm::vec3 torque, RotationSpace rotationSpace = RotationSpace::kLocalSpace)
	{
		if (rotationSpace == RotationSpace::kWorldSpace)
			torque = get_rot() * torque;

		this->angular_velocity_ += torque;
	}

	void apply_physics(float delta_time)
	{
		set_pos(get_pos() + this->velocity_ * delta_time);

		if (this->angular_velocity_ != glm::vec3(0.0f))
			rotate(angular_velocity_, glm::length(angular_velocity_) * delta_time);
	}


	// Transform Hierarchy.
	bool has_parent() const { return parent_ != nullptr; }
	Transform* get_parent() const { return parent_; }
	bool set_parent(Transform* const new_parent, const bool reset_local_values = false)
	{
		// -- Error Prevention --
		// We cannot set ourselves to our own parent.
		if (new_parent == this)
		{
			return false;
		}
		// We cannot set ourselves to the child of an object which we are the parent of (Whether immediately or not).
		if (new_parent->is_child_of(this))
		{
			return false;
		}

		// -- Parent Setting --

		// Remove ourselves from our old parent.
		if (parent_ != nullptr)
		{
			parent_->remove_child(this);
		}

		// Set our parent.
		parent_ = new_parent;

		// Notify our new parent that we are now it's child.
		parent_->add_child(this);

		if (reset_local_values)
		{
			// Zero our local pos/rot/scale values.
			this->set_local_pos(glm::vec3(0.0f));
			this->set_local_rot(glm::quat(glm::vec3(0.0f)));
			this->set_local_scale(glm::vec3(1.0f));
		}
		else
		{
			// Update our local pos/rot/scale values.
			std::cout << "Not Implemented Yet: Updating Local Values for Parenting" << std::endl;
			local_pos_;
			local_rot_;
			local_scale_;
		}

		return true;
	}
	/*
		Check if the passed transform is a parent of this transform (Either immediately or further up the hierarchy).
	*/
	bool is_child_of(const Transform* possible_parent) const
	{
		Transform* current_transform = this->parent_;

		// Check until we are at a root transform.
		while (current_transform != nullptr)
		{
			if (current_transform == possible_parent)
			{
				return true;
			}

			// Move up the hierarchy.
			current_transform = current_transform->get_parent();
		};

		return false;
	}


	int get_child_count() const { return children_.size(); }
	Transform* get_child(const int& index) const { return children_[index]; }
	/*
		Check if the passed transform is a child of this transform.
	*/
	bool has_child(const Transform* possible_child) const
	{
		// By using 'is_child_of', we only check parents, which there are likely to be less of than children.
		return possible_child->is_child_of(this);
	}

protected:
private:
	const glm::vec3 kWorldForward = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 kWorldRight = glm::vec3(1.0f, 0.0f, 0.0f);

	static float world_radius_;


	glm::vec3 loop_position_within_bounds(glm::vec3 world_pos)
	{
		float sqrDistanceFromOrigin = (world_pos.x * world_pos.x) + (world_pos.y * world_pos.y) + (world_pos.z * world_pos.z);

		if (sqrDistanceFromOrigin <= (world_radius_ * world_radius_))
			return world_pos;	// We are within the world radius.

		// We are outwith the world radius.
		// Loop our movement outside of the world radius, ensuring to account for if were > 2x the radius.

		glm::vec3 direction_from_origin = glm::normalize(world_pos);
		float distance_from_origin = std::sqrt(sqrDistanceFromOrigin);	// Alt: world_pos.length();

		// Calculate the fractional part of how far we're outside the radius (Accounts for distance > 2 x kWorldRadius).
		int distance_from_origin_int = (int)distance_from_origin;
		float outside_distance_remainder = (distance_from_origin_int % (int)world_radius_) + (distance_from_origin - (float)distance_from_origin_int);


		// Move to re-enter from the other side of the radius.
		return (-direction_from_origin * world_radius_) + (direction_from_origin * outside_distance_remainder);
	}


	// If Has Parent: Local Space. Else: World Space
	glm::vec3 local_pos_;
	glm::quat local_rot_;
	glm::vec3 local_scale_;

	glm::vec3 velocity_;
	glm::vec3 angular_velocity_;


	// Hierarchy.
	Transform* parent_;
	std::vector<Transform*> children_;


	void add_child(Transform* new_child) { children_.push_back(new_child); }
	/*
		Remove a child from a transform.
		Note: Doesn't unset the child's parent.
	*/
	void remove_child(Transform* child_to_remove) { children_.erase(std::remove(children_.begin(), children_.end(), child_to_remove), children_.end()); }


	void log_vec3(glm::vec3 vector) { std::cout << vector.x << ", " << vector.y << ", " << vector.z << std::endl; }
	void log_quat(glm::quat quaternion)
	{
		glm::vec3 vector = glm::eulerAngles(quaternion);
		std::cout << vector.x << ", " << vector.y << ", " << vector.z << std::endl;
	}

	// Quaternion Helper Functions.
	inline glm::quat diff(glm::quat to, glm::quat from) const { return to * glm::inverse(from); }
	inline glm::quat add(glm::quat start, glm::quat diff) const { return diff * start; }
};