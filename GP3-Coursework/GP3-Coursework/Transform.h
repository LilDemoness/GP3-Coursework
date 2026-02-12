#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <algorithm>
#include <vector>

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& rot = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f)) : parent_(nullptr), children_(std::vector<Transform*>())
	{
		this->pos_ = pos;
		this->rot_ = glm::quat(rot);
		this->scale_ = scale;
	}

	inline glm::mat4 get_model() const
	{
		glm::mat4 posMat = glm::translate(pos_);
		glm::mat4 rotMat = glm::toMat4(glm::inverse(rot_)); // Rotation quaternion needs to be inversed before converted otherwise rotations are inverted (Idk why, poor self implementation probably).
		glm::mat4 scaleMat = glm::scale(scale_);

		return posMat * rotMat * scaleMat;
	}


	// ----- Position -----
	inline glm::vec3 get_pos() const { return pos_; }
	inline void set_pos(const glm::vec3& new_pos)
	{
		glm::vec3 offset = new_pos - this->pos_;

		// Set our position.
		this->pos_ = new_pos;

		// Set our children's position.
		for (int i = 0; i < this->GetChildCount(); ++i)
		{
			this->get_child(i)->set_pos(this->get_child(i)->get_pos() + offset);
		}
	}
	inline void set_local_pos(const glm::vec3& new_local_pos)
	{
		if (parent_ == nullptr)
		{
			// We have no parent. Treat our parent position as the origin (World coords).
			this->set_pos(new_local_pos);
			return;
		}

		// We have a parent.
		// Determine our world offset from the parent (Includes rotation and scale).
		glm::vec3 desired_parent_offset = new_local_pos * parent_->get_rot(); // Rotation.
		desired_parent_offset *= parent_->get_scale();// Scale.

		// Set our position relative to their world position.
		this->set_pos(parent_->get_pos() + desired_parent_offset);
	}


	// ----- Rotation -----
	inline glm::quat get_rot() const { return rot_; }
	inline glm::vec3 get_euler_angles() const { return glm::eulerAngles(rot_); }
	inline void set_euler_angles(const float& pitch, const float& yaw, const float& roll) { this->set_rot(glm::quat(glm::vec3(pitch, yaw, roll))); }
	inline void set_euler_angles(const glm::vec3& new_rot) { this->set_rot(glm::quat(new_rot)); }
	inline void set_rot(const glm::quat& new_rot)
	{
		glm::quat rotation_difference = Diff(rot_, new_rot);

		// Set our rotation.
		this->rot_ = new_rot;

		// Update our children's rotation.
		for (int i = 0; i < this->GetChildCount(); ++i)
		{
			Transform* child = this->get_child(i);

			// --- Child Position ---
			// Determine position offset.
			glm::vec3 parent_to_child_vector = child->get_pos() - this->get_pos();

			// Calculate the child's new relative position.
			glm::vec3 new_parent_to_child_vector = rotation_difference * parent_to_child_vector;

			// Update the child's position relative to the parent.
			child->set_pos(this->get_pos() + new_parent_to_child_vector);


			// --- Child Rotation ---
			// Update the child's rotation.
			glm::quat new_rot = Add(glm::inverse(rotation_difference), child->get_rot());
			child->set_rot(new_rot);
		}
	}

	// ----- Scale -----
	inline glm::vec3 get_scale() const { return scale_; }
	inline void set_scale(glm::vec3& new_scale)
	{
		glm::vec3 old_scale = this->scale_;
		glm::vec3 scale_multiplier = new_scale / this->scale_;

		// Set our scale.
		this->scale_ = new_scale;

		// Update our children's scale & relative positions.
		for (int i = 0; i < this->GetChildCount(); ++i)
		{
			Transform* child = this->get_child(i);

			// Update child position.
			glm::vec3 position_offset = child->get_pos() - this->get_pos(); // Determine position offset.
			position_offset *= scale_multiplier; // Scale position offset by new scale.
			child->set_pos(this->get_pos() + position_offset);


			// Update child scale (Done after position to allow for correct calculations with hierarchies).
			// Multiply the child's scale by our new scale multiplier (New Scale / Old Scale).
			child->set_scale(child->get_scale() * scale_multiplier);
		}
	}


	// ----- Orientation -----
	inline glm::vec3 get_forward() const { return glm::normalize(kWorldForward * this->rot_); }
	inline glm::vec3 get_up() const { return glm::normalize(kWorldUp * this->rot_); }
	inline glm::vec3 get_right() const { return glm::normalize(kWorldRight * this->rot_); }



	enum RotationSpace
	{
		kLocalSpace,
		kWorldSpace
	};

	void Rotate(glm::vec3 axis, float angle, RotationSpace rotationSpace = RotationSpace::kLocalSpace)
	{
		if (rotationSpace == RotationSpace::kWorldSpace)
		{
			// World Space Rotation.
			// Transform the desired axis of rotation based on our current rotation.
			axis = rot_ * axis;
		}

		// Calculate and set our new rotation (Second Rot * First Rot).
		glm::quat orientation_quaternion = glm::angleAxis(angle, glm::normalize(axis));
		this->set_rot(Add(rot_, orientation_quaternion));
	}
	void RotateAroundPoint(glm::vec3 point, glm::vec3 rotation_axis, float angle)
	{
		// Rotate our position around the specified point.
		glm::quat position_orientation_quaternion = glm::angleAxis(angle, glm::normalize(rotation_axis));
		glm::vec3 rotated_point = point + (position_orientation_quaternion * (this->get_pos() - point));
		this->set_pos(rotated_point);

		// Transform the desired axis of rotation based on our current rotation.
		rotation_axis = rot_ * rotation_axis;

		// Apply our rotation.
		this->set_rot(Add(rot_, glm::angleAxis(-angle, glm::normalize(rotation_axis))));
	}


	// Transform Hierarchy.
	bool HasParent() const { return parent_ != nullptr; }
	Transform* get_parent() const { return parent_; }
	bool set_parent(Transform* new_parent, bool reset_position = false)
	{
		// -- Error Prevention --
		// We cannot set ourselves to our own parent.
		if (new_parent == this)
		{
			return false;
		}
		// We cannot set ourselves to the child of an object which we are the parent of (Whether immediately or not).
		if (new_parent->IsChildOf(this))
		{
			return false;
		}

		// -- Parent Setting --

		// Remove ourselves from our old parent.
		if (parent_ != nullptr)
		{
			parent_->RemoveChild(this);
		}

		// Set our parent.
		parent_ = new_parent;

		// Notify our new parent that we are now it's child.
		parent_->AddChild(this);

		// Zero our local position (If resired).
		if (reset_position)
		{
			this->set_local_pos(glm::vec3(0.0f));
		}

		return true;
	}
	/*
		Check if the passed transform is a parent of this transform (Either immediately or further up the hierarchy).
	*/
	bool IsChildOf(const Transform* possible_parent) const
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


	int GetChildCount() const { return children_.size(); }
	Transform* get_child(const int& index) const { return children_[index]; }
	/*
		Check if the passed transform is a child of this transform.
	*/
	bool HasChild(const Transform* possible_child) const
	{
		// By using 'IsChildOf', we only check parents, which there are likely to be less of than children.
		return possible_child->IsChildOf(this);

		/*// Check this transform.
		if (this == possible_child)
		{
			return true;
		}

		// Check each child.
		for (int i = 0; i < this->GetChildCount(); ++i)
		{
			if (this->get_child(i)->HasChild(possible_child))
			{
				return true;
			}
		}

		return false;*/
	}

protected:
private:
	const glm::vec3 kWorldForward = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec3 kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 kWorldRight = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 pos_;
	glm::quat rot_;
	glm::vec3 scale_;


	// Hierarchy.
	Transform* parent_;
	std::vector<Transform*> children_;


	void AddChild(Transform* new_child) { children_.push_back(new_child); }
	/*
		Remove a child from a transform.
		Note: Doesn't unset the child's parent.
	*/
	void RemoveChild(Transform* child_to_remove) { children_.erase(std::remove(children_.begin(), children_.end(), child_to_remove), children_.end()); }


	// Quaternion Helper Functions.
	inline glm::quat Diff(glm::quat to, glm::quat from) const { return to * glm::inverse(from); }
	inline glm::quat Add(glm::quat start, glm::quat diff) const { return diff * start; }
};