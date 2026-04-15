#pragma once
#include "InputManager.h"

std::unordered_map<SDL_Keycode, bool> InputManager::key_to_held_state_;
std::unordered_map<SDL_Keycode, Event<>> InputManager::key_to_on_pressed_event_map_;
std::unordered_map<SDL_EventType, Event<>> InputManager::sdl_event_to_triggered_event_map_;
Event<> InputManager::any_input_events_;
glm::vec2 InputManager::mouse_input_;
bool InputManager::receiving_mouse_input_ = false;

void InputManager::clear()
{
	for (auto val : key_to_on_pressed_event_map_)
		val.second.unsubscribe_all();
	for (auto val : sdl_event_to_triggered_event_map_)
		val.second.unsubscribe_all();
	
	any_input_events_.unsubscribe_all();
}




void InputManager::process_input()
{
	bool any_input_events = false;

	// Get and process events
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		//auto it = sdl_event_to_triggered_event_map_.find(evnt);
		//if (it != sdl_event_to_triggered_event_map_.end())
		//	it->second.invoke();
		if (sdl_event_to_triggered_event_map_.find((SDL_EventType)evnt.type) != sdl_event_to_triggered_event_map_.end())
			sdl_event_to_triggered_event_map_[(SDL_EventType)evnt.type].invoke();

		if (evnt.type != SDL_KEYDOWN && evnt.type != SDL_KEYUP)
			continue;

		process_input_event(evnt);
		any_input_events = true;
	}

	process_general_input();

	if (any_input_events)
	{
		any_input_events_.invoke();
	}
}
void InputManager::process_input_event(const SDL_Event& event)
{
	if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
		return;	// Not an input event.

	SDL_Keycode keycode = event.key.keysym.sym;

	if (key_to_held_state_.find(keycode) != key_to_held_state_.end())
	{
		if (event.type == SDL_KEYDOWN)
		{
			//auto event_it = key_to_on_pressed_event_map_.find(keycode);
			if (key_to_on_pressed_event_map_.find(keycode) != key_to_on_pressed_event_map_.end())
				key_to_on_pressed_event_map_[keycode].invoke();
		}

		key_to_held_state_[keycode] = (event.type == SDL_KEYDOWN);
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
	key_to_held_state_.try_emplace(keycode, false);
}

void InputManager::register_input_event(const SDL_Keycode keycode, std::function<void()> callback)
{
	std::cout << "Register Input Event: " << keycode << std::endl;
	register_input(keycode);

	key_to_on_pressed_event_map_.try_emplace(keycode, Event<>());
	key_to_on_pressed_event_map_[keycode].subscribe(callback);
}
void InputManager::deregister_input_event(const SDL_Keycode keycode, std::function<void()> callback)
{
	key_to_on_pressed_event_map_[keycode].unsubscribe(callback);
}
void InputManager::register_event(const SDL_EventType event_identifier, std::function<void()> callback)
{
	sdl_event_to_triggered_event_map_.try_emplace(event_identifier, Event<>());
	sdl_event_to_triggered_event_map_[event_identifier].subscribe(callback);
}
void InputManager::deregister_event(const SDL_EventType event_identifier, std::function<void()> callback)
{
	sdl_event_to_triggered_event_map_[event_identifier].unsubscribe(callback);
}


const bool InputManager::get_key_held(const SDL_Keycode keycode)
{
	auto iterator = key_to_held_state_.find(keycode);
	if (iterator == key_to_held_state_.end())
		return false;

	return iterator->second;
}