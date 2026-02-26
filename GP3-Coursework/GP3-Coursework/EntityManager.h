#pragma once

#include <vector>

#include "Entity.h"
#include "Pool.h"

class EntityManager
{
public:
	EntityManager* get_instance();
	
	Entity create();
	void destroy();

	Entity get();

	template<typename C, typename = typename std::enable_if<!std::is_const<C>::value>::type>
	ComponentHandle<C> get_component_for_entity(Entity::Id id)
	{
		assert_if_invalid(id);
		std::size_t family = component_family<C>();

		// Invalid family. We don't need to bother checking our BitMask as we're returning a nullptr anyway.
		if (family >= component_pools_.size())
			return ComponentHandle<C>();

		Pool* pool = component_pools_[family];
		if (!pool || !entity_component_masks_[id.index()][family])
			return ComponentHandle<C>(); // No component of the desired type on the entity with this Id. Return nullptr.
		// Found the desired component.
		return ComponentHandle<C>(this, id);
	}

protected:
private:
	EntityManager();
	~EntityManager();


	// Each element in 'component_pools_' corresponds to a Pool for a component.
	// Index for the vector is the Component::family().
	std::vector<Pool*> component_pools_;
	// Bitmask of components within each entity. Index for the vector is the Entity::Id.
	std::vector<std::bitset<Entity::kMaxComponents>> entity_component_masks_;


	// Count of Ids that exist (Including free ones).
	uint32_t index_counter_ = 0;
	// Vector of available entity Ids.
	std::vector<Entity::Id> free_list_;


	void assert_if_invalid(Entity::Id id) const;
	// Returns true if the given Entity Id is still valid.
	bool valid_id(Entity::Id id) const;
};