#pragma once
#include "SystemManager.h"


SystemManager::SystemManager(EntityManager& entity_manager) :
	entity_manager_(entity_manager)
{}


template<typename S>
void SystemManager::add(std::shared_ptr<S> system)
{
	systems_.insert(std::make_pair(S::family(), system));
}

template<typename S, typename ... Args>
std::shared_ptr<S> SystemManager::add(Args&& ... args)
{
	std::shared_ptr<S> s(new S(std::forward<Args>(args) ...));
	add(s);
	return s;
}


template<typename S>
std::shared_ptr<S> SystemManager::get_system()
{
	auto iterator = systems_.find(S::family());
	assert(iterator != systems_.end());

	return iterator == systems_.end() ? std::shared_ptr<S>() : std::shared_ptr<S>(std::static_pointer_cast<S>(iterator->second));
}


template<typename S>
void SystemManager::update(float delta_time)
{
	assert(is_initialised_ && "SystemManager::configure() not called.");
	std::shared_ptr<S> system = get_system<S>();
	system->update(entity_manager_, delta_time);
}
void SystemManager::update_all(float delta_time)
{
	assert(is_initialised_ && "SystemManager::configure() not called");
	for (auto& pair : systems_)
	{
		pair.second->update(entity_manager_, delta_time);
	}
}


void SystemManager::configure()
{
	for (auto& pair : systems_)
	{
		pair.second->configure(entity_manager_);
	}

	is_initialised_ = true;
}