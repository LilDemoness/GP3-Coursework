#pragma once

#include "Entity.h"

// ComponentHandle<C> is a wrapper around an instance of a component.
//
// It provides safe access to components.
// The handle will be invalidated under the following conditions:
// - If a component is removed from its host entity.
// - If its host entity is destroyed.
template<typename C, typename EM>
class ComponentHandle
{
public:
	typedef C ComponentType;

	ComponentHandle() : manager(nullptr) {}

	// Returns rrue if the ComponentHandle is valid.
	bool is_valid() const;
	operator bool() const;

	C* operator-> ();
	const C* operator-> () const;

	C& operator* ();
	const C& operator* () const;

	C* get();
	const C* get() const;


	// Remove the component from its entity and destroy it.
	void remove();


	// Returns the entity associated with the component.
	Entity get_entity();


	bool operator==(const ComponentHandle<C>& other) const	{ return manager_ == other.manager_ && id_ == other.id_; }
	bool operator!= (const ComponentHandle<C>& other) const { return !(*this == other); }


private:
	friend class EntityManager;

	ComponentHandle(EM* manager, Entity::Id id) :
		manager_(manager),
		id_(id)
	{ }

	EM* manager_;
	Entity::Id id_;
};