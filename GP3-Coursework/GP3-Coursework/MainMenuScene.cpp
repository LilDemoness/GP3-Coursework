#pragma once
#include "MainMenuScene.h"

MainMenuScene::MainMenuScene()
{}
MainMenuScene::~MainMenuScene()
{}


void MainMenuScene::enter()
{
	InputManager::get_instance().register_any_input_event(std::bind(&MainMenuScene::start_game, this));
}
void MainMenuScene::update(float delta_time)
{}
void MainMenuScene::draw(DisplayFacade* display_facade)
{
	TextRenderer::render_text("Main Menu", display_facade->get_width() / 2.0f, display_facade->get_height() - 100.0f, 2.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
	TextRenderer::render_text("Press any key to start", display_facade->get_width() / 2.0f, display_facade->get_height() / 2.0f, 1.0f, glm::vec3(1.0f), TextRenderer::TextJustification::kCentreAligned);
}

void MainMenuScene::on_exit_fulfilled()
{
	InputManager::get_instance().deregister_any_input_event(std::bind(&MainMenuScene::start_game, this));
}


void MainMenuScene::start_game()
{
	on_exit_requested.invoke(Scene::GameMode::kGameplay);
}