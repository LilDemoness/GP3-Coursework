#pragma once

#include <memory>
#include <unordered_map>

#include "System.h"
#include "EntityManager.h"


class SystemManager
{
public:
	SystemManager(EntityManager& entity_manager);


	// Add a System to the SystemManager.
	// Must be called before the System can be used.
	//
	// E.g.
	// std::shared_ptr<MovementSystem> movement = std::make_shared<MovementSystem>();
	// system_manager_.add(movement);
	template<typename S>
	void add(std::shared_ptr<S> system);

	// Add a System to the SystemManager.
	// Must be called before the System can be used.
	//
	// E.g.
	// auto movement = system.add<MovementSystem>();
	template<typename S, typename ... Args>
	std::shared_ptr<S> add(Args&& ... args);


	// Retrieve the registered System instance, if any.
	// @return System instance or empty std::shared_ptr<S>.
	template<typename S>
	std::shared_ptr<S> get_system();


	// Calls System.update() for a registered system type.
	template<typename S>
	void update(float delta_time);

	// Calls System::update() on all registered systems.
	//
	// The order which registered systems are updated is arbitrary but consistent,
	// meaning that the order cannot be specified, but remains the same so long as
	// no systems are added or removed.
	// 
	// If the order the systems update in is important, use SystemManager::update()
	// to manually specify the update order.
	void update_all(float delta_time);


	// Configure the System Manager. Call after adding all systems.
	void configure();


private:
	bool is_initialised_ = false;
	EntityManager& entity_manager_;
	std::unordered_map<BaseSystem::Family, std::shared_ptr<BaseSystem>> systems_;
};