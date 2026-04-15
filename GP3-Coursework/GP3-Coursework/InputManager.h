#pragma once

#include <SDL\SDL.h>
#include <glm\glm.hpp>
#include <iostream>

#include <unordered_map>
#include <unordered_set>

#include "Event.h"


// Once SDL_Keycode values are registered, stores whether any given key is being held this frame.
class InputManager
{
public:
	static void process_input();

	
	static void register_input(const std::vector<SDL_Keycode> keycodes);
	static void register_input(const SDL_Keycode keycode);

	static void register_input_event(const SDL_Keycode keycode, std::function<void()> callback);
	static void deregister_input_event(const SDL_Keycode keycode, std::function<void()> callback);

	static inline void register_any_input_event(std::function<void()> callback) { any_input_events_.subscribe(callback); }
	static inline void deregister_any_input_event(std::function<void()> callback)
	{
		any_input_events_.unsubscribe(callback);
	}

	static void register_event(const SDL_EventType event_identifier, std::function<void()> callback);
	static void deregister_event(const SDL_EventType event_identifier, std::function<void()> callback);


	static const bool get_key_held(const SDL_Keycode keycode);

	static void clear();

private:
	InputManager() = delete;
	~InputManager() = delete;
	InputManager(InputManager& other) = delete;
	InputManager& operator=(const InputManager& other) = delete;

	static void process_input_event(const SDL_Event& event);
	static void process_general_input();


	static std::unordered_map<SDL_Keycode, bool> key_to_held_state_;
	static std::unordered_map<SDL_Keycode, Event<>> key_to_on_pressed_event_map_;
	static std::unordered_map<SDL_EventType, Event<>> sdl_event_to_triggered_event_map_;
	static Event<> any_input_events_;
	static bool receiving_mouse_input_;
	static glm::vec2 mouse_input_;
};