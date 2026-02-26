#pragma once

#include <cstddef>
#include <new>
#include <cstdlib>

// Component implementations should inherit from this.
//
// Components MUST provide a no-argument constructor
// Components SHOULD provide convenience constructors for initialising on assignment to an Entity::Id.
//
// This is a struct to imply that Components should be data-only.
//
// Usage:
// struct Position : public Component<Position>
// {
//     Position(float x = 0.0f, float y = 0.0f) : 
//         x_(x),
//         y_(y)
//     {}
//
//     float x, y;
//  }
//
// Family is used for registration.
template <typename Derived>
struct Component : public BaseComponent
{
public:
	typedef ComponentHandle<Derived> Handle;
	typedef ComponentHandle<const Derived, const EntityManager> ConstHandle;

private:
	friend class EntityManager;

	/// Used internally for registration.
	static Family family();
};


// Base component class, only used for insertion into collections.
// Family is used for registration.
struct BaseComponent
{
public:
	typedef std::size_t Family;


	// NOTE: Component memory is *always* managed by the EntityManager.
	// Use Entity::destroy() instead.
	void operator delete(void* p) { fail(); }
	void operator delete[](void* p) { fail(); }

protected:
	static void fail()
	{
#if defined(HAS_EXCEPTIONS) || defined(__EXCEPTIONS)
		throw std::bad_alloc();
#else
		std::abort();
#endif
	}

	static Family family_counter_;
};