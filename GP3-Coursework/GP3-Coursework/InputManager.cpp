#pragma once
#include "InputManager.h"
#include <iostream>

InputManager::InputManager() :
	receiving_mouse_input_(false)
{ }
InputManager::~InputManager()
{ }


InputManager& InputManager::get_instance()
{
	static InputManager instance;
	return instance;
}


void InputManager::process_input(const SDL_Event& event)
{
	// Key Pressed States.
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
	{
		if (key_to_held_state_.find(event.key.keysym.sym) != key_to_held_state_.end())
		{
			key_to_held_state_[event.key.keysym.sym] = event.type == SDL_KEYDOWN;
		}
	}


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


const bool InputManager::get_key_held(const SDL_Keycode keycode) const
{
	auto iterator = key_to_held_state_.find(keycode);
	if (iterator == key_to_held_state_.end())
		return false;

	return iterator->second;
}