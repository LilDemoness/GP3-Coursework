#pragma once

#include "Shader.h"
#include "ShaderManager.h"

#define WORLD_BORDER_SHADER_PATH "..\\res\\WorldBorderShader"
#define WORLD_BORDER_SHADER_TAG "WorldBorderShader"

class WorldBorderVisuals
{
public:
	static void initialise_world_border(float world_radius);
	static void draw_world_border(const glm::vec3& player_position);

	static void set_world_radius(const float world_radius);

private:
	static float world_radius_;
	static GLuint empty_vao_;
};