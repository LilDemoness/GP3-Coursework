#pragma once

#include <SDL\SDL.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include "Event.h"


// Once SDL_Keycode values are registered, stores whether any given key is being held this frame.
class InputManager
{
public:
	static InputManager& get_instance();

	void process_input();

	void register_input(const std::vector<SDL_Keycode> keycodes);
	void register_input(const SDL_Keycode keycode);

	void register_input_event(const SDL_Keycode keycode, std::function<void()> callback);
	void deregister_input_event(const SDL_Keycode keycode, std::function<void()> callback);
	inline void register_any_input_event(std::function<void()> callback) { any_input_events_.subscribe(callback); }
	inline void deregister_any_input_event(std::function<void()> callback) { any_input_events_.unsubscribe(callback); }

	const bool get_key_held(const SDL_Keycode keycode) const;

private:
	InputManager();
	~InputManager();

	void process_input_event(const SDL_Event& event);
	void process_general_input();


	std::unordered_map<SDL_Keycode, bool> key_to_held_state_;
	std::unordered_map<SDL_Keycode, Event<>> key_to_on_pressed_event_map_;
	Event<> any_input_events_;
	bool receiving_mouse_input_;
	glm::vec2 mouse_input_;
};