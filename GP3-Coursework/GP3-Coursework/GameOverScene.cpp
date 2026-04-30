#pragma once
#include "GameOverScene.h"


GameOverScene::GameOverScene()
{}
GameOverScene::~GameOverScene()
{}


void GameOverScene::enter(DisplayFacade* display_facade)
{
	InputManager::register_input_event(SDLK_r, std::bind(&GameOverScene::restart_game, this));
	InputManager::register_input_event(SDLK_RETURN, std::bind(&GameOverScene::quit_game, this));
}
void GameOverScene::update(float delta_time)
{}
void GameOverScene::draw(DisplayFacade* display_facade)
{
	float centre_x = display_facade->get_width() / 2.0f;
	float centre_y = display_facade->get_height() / 2.0f;

	// We set up our text position in 1280x720p. This should make it resolution-independent.
	float height_offset_multiplier = display_facade->get_height() / 720.0f;
	float scale_multiplier = display_facade->get_height() / 720.0f;	// Assumes 16:9 ratio.


	TextRenderer::render_text(display_facade, "Game Over", centre_x, centre_y + (260.0f * height_offset_multiplier), 2.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	TextRenderer::render_text(display_facade, "Score", centre_x, centre_y + (15.0f * height_offset_multiplier), 1.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(display_facade, std::to_string(score_), centre_x, centre_y - (25.0f * height_offset_multiplier), 1.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	TextRenderer::render_text(display_facade, "Press [r] to restart", centre_x, centre_y - (240.0f * height_offset_multiplier), 1.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(display_facade, "Press [enter] to quit", centre_x, centre_y - (300.0f * height_offset_multiplier), 1.0f * scale_multiplier, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
}

void GameOverScene::on_exit_fulfilled()
{
	InputManager::deregister_input_event(SDLK_r, std::bind(&GameOverScene::restart_game, this));
	InputManager::deregister_input_event(SDLK_RETURN, std::bind(&GameOverScene::quit_game, this));
}


void GameOverScene::quit_game() { on_exit_requested.invoke(Scene::GameMode::kExitGame); }
void GameOverScene::restart_game() { on_exit_requested.invoke(Scene::GameMode::kGameplay); }