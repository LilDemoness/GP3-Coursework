#pragma once
#include "WorldBorderVisuals.h"


GLuint WorldBorderVisuals::empty_vao_ = 0;
void WorldBorderVisuals::initialise_world_border(float world_radius)
{
	glGenVertexArrays(1, &empty_vao_);
	ShaderManager::load_shader(WORLD_BORDER_SHADER_TAG, WORLD_BORDER_SHADER_PATH);
	set_world_radius(world_radius);
}
void WorldBorderVisuals::draw_world_border(const glm::vec3& player_position)
{
	std::shared_ptr<Shader> world_border_shader = ShaderManager::get_shader(WORLD_BORDER_SHADER_TAG);
	world_border_shader->set_vec3("player_position_ws", player_position);
	world_border_shader->bind();

	glBindVertexArray(empty_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
}


float WorldBorderVisuals::world_radius_ = 0.0f;
void WorldBorderVisuals::set_world_radius(const float world_radius)
{
	world_radius_ = world_radius;
	ShaderManager::get_shader(WORLD_BORDER_SHADER_TAG)->set_float("world_radius", world_radius);
}