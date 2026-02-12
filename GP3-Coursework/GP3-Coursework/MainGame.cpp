#pragma once

#include "MainGame.h"


MainGame::MainGame()
	: game_display_("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT),
	  game_state_(GameState::PLAY), counter_(0.0f),
	  my_camera_(glm::vec3(0, 0, -5), 70.0f, (float)game_display_.get_width() / game_display_.get_height(), 0.01f, 1000.0f),
	  texture_("..\\res\\bricks.jpg"),
      mesh_1_(Mesh::CreateTriangleMesh()),
      mesh_2_("..\\res\\monkey3.obj")
{
	ShaderManager::get_instance().LoadShader("DefaultShader", "..\\res\\shader");
}
MainGame::~MainGame()
{
	ShaderManager::get_instance().Clear();
}

void MainGame::Run()
{
	InitSystems(); 
	GameLoop();
}

void MainGame::InitSystems()
{
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering.
	glEnable(GL_CULL_FACE);  // Enable face culling.
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Set clear color to black.
}

void MainGame::GameLoop()
{
	while (game_state_ != GameState::EXIT)
	{
		ProcessInput();
		DrawGame();
	}
}

void MainGame::ProcessInput()
{
	// Get and process events
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
			case SDL_QUIT:
				game_state_ = GameState::EXIT;
				break;
		}
	}
	
}


void MainGame::DrawGame()
{
	game_display_.ClearDisplay();

	std::shared_ptr<Shader> active_shader = ShaderManager::get_instance().GetActiveShader();
	active_shader->Bind();

	texture_.Bind(0);
	transform_.set_rot(glm::vec3(0.0,counter_ * 2, 0.0));
	transform_.set_scale(glm::vec3(1.0, 1.0, 1.0));


	transform_.set_pos(glm::vec3(0.0,0.0, 0.0));
	active_shader->Update(transform_, my_camera_);
	mesh_1_.Draw();

	transform_.set_pos(glm::vec3(2.0, 0.0, 0.0));
	active_shader->Update(transform_, my_camera_);
	mesh_2_.Draw();


	counter_ = counter_ + 0.01f;
				
	//glEnableClientState(GL_COLOR_ARRAY); 
	//glEnd();

	game_display_.SwapBuffers();
} 