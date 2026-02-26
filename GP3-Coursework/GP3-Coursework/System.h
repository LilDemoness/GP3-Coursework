#pragma once

#include "EntityManager.h"


// Don't use for deriving systems. Instead use System<Derived>
// This base class exists so we can get references without needing to specify types.
class BaseSystem
{
public:
	typedef std::size_t Family;
	virtual ~BaseSystem() {}

	// Called once all Systems have been added to the SystemManager.
	virtual void configure(EntityManager& entities) {}

	// Apply System behaviour.
	// Called every frame.
	virtual void update(EntityManager& entities, float delta_time) = 0;

	static Family family_counter_;
};


// Class to be used when implementing systems.
// 
// Example:
// struct MovementSystem : public System<MovementSystem>
// {
//		void update(EntityManager& entities, float delta_time) { /*...*/ }
// }
template<typename Derived>
class System
{
public:
	virtual ~System() {}

private:
	friend class SystemManager;

	static Family family()
	{
		static Family family = family_counter_++;
		return family;
	}
};