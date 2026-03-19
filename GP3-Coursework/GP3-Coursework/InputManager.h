#pragma once

#include <SDL\SDL.h>
#include <glm\glm.hpp>
#include <unordered_map>


// Once SDL_Keycode values are registered, stores whether any given key is being held this frame.
class InputManager
{
public:
	static InputManager& get_instance();

	void process_input(const SDL_Event& event);

	void register_input(const std::vector<SDL_Keycode> keycodes);
	void register_input(const SDL_Keycode keycode);

	const bool get_key_held(const SDL_Keycode keycode) const;

private:
	InputManager();
	~InputManager();


	std::unordered_map<SDL_Keycode, bool> key_to_held_state_;
	bool receiving_mouse_input_;
	glm::vec2 mouse_input_;
};