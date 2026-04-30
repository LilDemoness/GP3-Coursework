#pragma once
#include "MainMenuScene.h"

MainMenuScene::MainMenuScene()
{}
MainMenuScene::~MainMenuScene()
{}


void MainMenuScene::enter(DisplayFacade* display_facade)
{
	InputManager::register_input_event(SDLK_RETURN, std::bind(&MainMenuScene::start_game, this));
}
void MainMenuScene::update(float delta_time)
{}
void MainMenuScene::draw(DisplayFacade* display_facade)
{
	// We set up our text position in 1280x720p. This should make it resolution-independent.
	float height_offset_multiplier = display_facade->get_height() / 720.0f;
	float scale_multiplier = display_facade->get_height() / 720.0f;	// Assumes 16:9 ratio.

	TextRenderer::render_text(display_facade, "Main Menu", display_facade->get_width() / 2.0f, display_facade->get_height() - (100.0f * height_offset_multiplier), 2.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(display_facade, "Press [enter] to start", display_facade->get_width() / 2.0f, display_facade->get_height() / 2.0f, 1.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
}

void MainMenuScene::on_exit_fulfilled()
{
	InputManager::deregister_input_event(SDLK_RETURN, std::bind(&MainMenuScene::start_game, this));
}


void MainMenuScene::start_game()
{
	on_exit_requested.invoke(Scene::GameMode::kGameplay);
}