#pragma once
#include "InputManager.h"
#include <iostream>

InputManager::InputManager() :
	receiving_mouse_input_(false)
{ }
InputManager::~InputManager()
{
	for (auto val : key_to_on_pressed_event_map_)
	{
		val.second.unsubscribe_all();
	}
}


InputManager& InputManager::get_instance()
{
	static InputManager instance;
	return instance;
}


void InputManager::process_input_event(const SDL_Event& event)
{
	// Key Pressed States.
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
	{
		SDL_Keycode keycode = event.key.keysym.sym;

		if (key_to_held_state_.find(keycode) != key_to_held_state_.end())
		{
			if (event.type == SDL_KEYDOWN && !key_to_held_state_[keycode] && key_to_on_pressed_event_map_.find(keycode) != key_to_on_pressed_event_map_.end())
				key_to_on_pressed_event_map_[keycode].invoke();

			key_to_held_state_[keycode] = event.type == SDL_KEYDOWN;
		}
	}
}
void InputManager::process_general_input()
{
	// Mouse Input.
	if (receiving_mouse_input_)
	{
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		mouse_input_ = glm::vec2(x, y);
	}
	else
	{
		mouse_input_ = glm::vec2(0.0f);
	}
}


void InputManager::register_input(const std::vector<SDL_Keycode> keycodes)
{
	for (int i = 0; i < keycodes.size(); ++i)
		register_input(keycodes[i]);
}
void InputManager::register_input(const SDL_Keycode keycode)
{
	std::cout << "Register Input: " << keycode << std::endl;
	key_to_held_state_.insert({ keycode, false });
}

void InputManager::register_input_event(const SDL_Keycode keycode, std::function<void()> callback)
{
	std::cout << "Register Input Event: " << keycode << std::endl;
	register_input(keycode);

	if (key_to_on_pressed_event_map_.find(keycode) == key_to_on_pressed_event_map_.end())
		key_to_on_pressed_event_map_.insert({ keycode, Event<>() });
	key_to_on_pressed_event_map_[keycode].subscribe(callback);
}
void InputManager::deregister_input_event(const SDL_Keycode keycode, std::function<void()> callback)
{
	key_to_on_pressed_event_map_[keycode].unsubscribe(callback);
}


const bool InputManager::get_key_held(const SDL_Keycode keycode) const
{
	auto iterator = key_to_held_state_.find(keycode);
	if (iterator == key_to_held_state_.end())
		return false;

	return iterator->second;
}