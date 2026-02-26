#pragma once
#include "ComponentHandle.h"

template <typename C, typename EM>
inline bool ComponentHandle<C, EM>::is_valid() const
{
	return manager_ && manager_->valid(id_) && manager_->template has_component<C>(id_);
}
template <typename C, typename EM>
inline ComponentHandle<C, EM>::operator bool() const
{
	return is_valid();
}


template <typename C, typename EM>
inline C* ComponentHandle<C, EM>::operator-> ()
{
	assert(is_valid());
	return manager_->template get_component_ptr<C>(id_);
}
template <typename C, typename EM>
inline const C* ComponentHandle<C, EM>::operator-> () const
{
	assert(is_valid());
	return manager_->template get_component_ptr<C>(id_);
}

template <typename C, typename EM>
inline C& ComponentHandle<C, EM>::operator* ()
{
	assert(is_valid());
	return *manager_->template get_component_ptr<C>(id_);
}

template <typename C, typename EM>
inline const C& ComponentHandle<C, EM>::operator* () const
{
	assert(is_valid());
	return *manager_->template get_component_ptr<C>(id_);
}

template <typename C, typename EM>
inline C* ComponentHandle<C, EM>::get()
{
	assert(is_valid());
	return manager_->template get_component_ptr<C>(id_);
}
template <typename C, typename EM>
inline const C* ComponentHandle<C, EM>::get() const
{
	assert(is_valid());
	return manager_->template get_component_ptr<C>(id_);
}


template <typename C, typename EM>
inline void ComponentHandle<C, EM>::remove()
{
	assert(is_valid());
	manager_->template remove<C>(id_);
}


template <typename C, typename EM>
inline Entity ComponentHandle<C, EM>::get_entity()
{
	assert(is_valid());
	return manager_->get(id_);
}