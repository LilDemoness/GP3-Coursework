#pragma once
#include "Skybox.h"

Skybox::Skybox()
{
	ShaderManager::get_instance().load_shader(SKYBOX_SHADER_TAG, SKYBOX_SHADER_PATH);

	for (int i = 0; i < kSkyboxCount; ++i)
	{
		skybox_rotations_[i] = 0.0f;
		skybox_textures_[i] = Cubemap::create_cubemap_texture(kSkyboxImagesPaths[i], SKYBOX_IMAGE_EXTENSION);
	}


	// Initialise skybox rendering variables.
	glGenVertexArrays(1, &vertex_array_object_);
	glGenBuffers(1, &vertex_buffer_object_);

	glBindVertexArray(vertex_array_object_);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}
Skybox::~Skybox()
{ }


void Skybox::update(float delta_time)
{
	constexpr float MAX_RADIAN_VALUE = glm::radians(360.0f);

	for (int i = 0; i < kSkyboxCount; ++i)
	{
		skybox_rotations_[i] += skybox_rotation_rates_[i] * delta_time;

		if (skybox_rotations_[i] > MAX_RADIAN_VALUE)
			skybox_rotations_[i] -= MAX_RADIAN_VALUE;
	}
}
void Skybox::draw(const Camera& camera)
{
	// Change our depth function so that we draw the skybox behind everything.
	glDepthFunc(GL_LEQUAL);

	// Prepare to draw the skybox cube.
	glBindVertexArray(vertex_array_object_);

	// Retrieve our shader
	std::shared_ptr<Shader> skybox_shader = ShaderManager::get_instance().get_shader(SKYBOX_SHADER_TAG);
	skybox_shader->bind();
	skybox_shader->update_matrices_ubo_for_skybox(camera);

	// Update our shader uniforms.
	for (int i = 0; i < kSkyboxCount; ++i)
	{
		std::string array_append_string = "[" + std::to_string(i) + "]";

		// Bind the skybox cubemap.
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_textures_[i]->get_texture_id());
		skybox_shader->set_int("skyboxes" + array_append_string, i);

		// Update the skybox rotation.
		skybox_shader->set_float("skybox_rotations" + array_append_string, skybox_rotations_[i]);
	}

	// Draw the Skybox Cube.
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	// Reset the depth function.
	glDepthFunc(GL_LESS);
}