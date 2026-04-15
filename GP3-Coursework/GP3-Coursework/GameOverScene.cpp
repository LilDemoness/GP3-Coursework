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
	TextRenderer::render_text("Game Over", display_facade->get_width() / 2.0f, display_facade->get_height() - 100.0f, 2.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	TextRenderer::render_text("Score", display_facade->get_width() / 2.0f, display_facade->get_height() / 2.0f + 15.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text(std::to_string(score_), display_facade->get_width() / 2.0f, display_facade->get_height() / 2.0f - 25.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);

	TextRenderer::render_text("Press [r] to restart", display_facade->get_width() / 2.0f, 120.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text("Press [enter] to quit", display_facade->get_width() / 2.0f, 60.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
}

void GameOverScene::on_exit_fulfilled()
{
	InputManager::deregister_input_event(SDLK_r, std::bind(&GameOverScene::restart_game, this));
	InputManager::deregister_input_event(SDLK_RETURN, std::bind(&GameOverScene::quit_game, this));
}


void GameOverScene::quit_game() { on_exit_requested.invoke(Scene::GameMode::kExitGame); }
void GameOverScene::restart_game() { on_exit_requested.invoke(Scene::GameMode::kGameplay); }