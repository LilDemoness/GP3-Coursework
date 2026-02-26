#pragma once

#include <cstdint>
#include <bitset>
#include "EntityManager.h"

class EntityManager;

class Entity
{
public:
	// Wrapper for an Entity Id, allowing for easy swapping of the type.
	struct Id
	{
		Id() : id_(0) {}
		Id(uint32_t id) : id_(id) {}

		uint32_t id() const { return id_; }

		bool operator== (const Id& other) const { return id_ == other.id_; }
		bool operator!= (const Id& other) const { return id_ != other.id_; }
		bool operator< (const Id& other) const	{ return id_ < other.id_; }

	private:
		uint32_t id_;
	};

	static const size_t kMaxComponents = 16;

	
	// Constructors.
	Entity() = default;
	Entity(Entity::Id id) : id_(id) { }
	Entity(const Entity& other) = default;
	Entity& operator= (const Entity& other) = default;


	Id id() const { return id_; }
	std::bitset<Entity::kMaxComponents> component_mask() const;

private:
	Id id_;
};