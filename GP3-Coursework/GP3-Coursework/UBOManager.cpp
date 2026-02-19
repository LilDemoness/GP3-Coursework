#pragma once

#include "UBOManager.h"

UBOManager::UBOManager() : ubos_()
{}
UBOManager::~UBOManager()
{}


// Returns a singleton instance of the UBOManager.
UBOManager& UBOManager::get_instance()
{
	static UBOManager instance;
	return instance;
}


// Creates a new Uniform Buffer Object (UBO).
// 
// Generates a UBO, allocates memory for it, and binds it to a specified binding point.</br>
// The Binding Point allows shaders to access the UBO's data efficiently.
//
// @param tag A unique identifier for the UBO.
// @param size The total size of the UBO, in bytes.
// @param binding_point The binding index that links the UBO to shaders.
void UBOManager::CreateUBO(const std::string& tag, const size_t size, const GLuint binding_point)
{
	// Check if a UBO with this tag already exists.
	if (ubos_.find(tag) != ubos_.end())
	{
		std::cerr << "UBO already exists for tag: " << tag << std::endl;
		return;
	}

	// Create & Setup the UBO.
	GLuint ubo;
	glGenBuffers(1, &ubo); // Generate a buffer for the UBO. 
	glBindBuffer(GL_UNIFORM_BUFFER, ubo); // Bind the buffer for modification.
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // Allocate memory for the UBO.
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo); // Link UBO to binding point.
	glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind UBO after setup.

	// Store the UBO in the manager.
	ubos_[tag] = UBOData(ubo, size, binding_point);
}


// Updates the contents of an existing Uniform Buffer Object (UBO) at a specific offset.</br>
// Allows dynamic updates to GPU memory without reallocating buffers.
//
// @param tag The unique identifier of the UBO.
// @param offset The byte offset within the UBO to start updating data.
// @param data Pointer to the data being sent to the GPU.
// @param data_size The size of the data being sent, in bytes.
void UBOManager::UpdateUBOData(const std::string& tag, const size_t offset, const void* data, const size_t data_size)
{
	// Check if a UBO with this tag already exists.
	if (ubos_.find(tag) == ubos_.end())
	{
		std::cerr << "No UBO exists for tag: " << tag << std::endl;
		return;
	}

	// Bind the UBO and update the specified section.
	glBindBuffer(GL_UNIFORM_BUFFER, ubos_[tag].buffer_id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, data_size, data);
	// Unbind after update.
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


// Binds a Uniform Buffer Object (UBO) to a shader uniform block to ensure efficient UBO data access.
//
// @param ubo_tag The identifier of the UBO in the UBOManager.
// @param shader_id The OpenGL shader program ID.
// @param ubo_name The name of the uniform block in the shader.
void UBOManager::BindUBOToShader(const std::string& ubo_tag, const GLuint shader_id, const std::string& ubo_name)
{
	// Check if a UBO with this tag already exists.
	if (ubos_.find(ubo_tag) == ubos_.end())
	{
		std::cerr << "No UBO exists for tag: " << ubo_tag << std::endl;
		return;
	}

	// Get the uniform block index in the shader.
	GLuint block_index = glGetUniformBlockIndex(shader_id, ubo_name.c_str());
	if (block_index == GL_INVALID_INDEX)
	{
		std::cerr << "UBO block '" << ubo_name << "' not found in shader with ID: " << shader_id << std::endl;
		return;
	}

	// Bind the uniform block to the UBO's binding point.
	glUniformBlockBinding(shader_id, block_index, ubos_[ubo_tag].binding_point);
}


// Deletes all stored Uniform Buffer Objects (UBOs) and clears the manager.
// This frees GPU Memory and should be used when cleaning up the application or unloading assets.
void UBOManager::Clear()
{
	// Delete all UBO Buffers.
	for (auto const& [tag, ubo_data] : ubos_)
	{
		glDeleteBuffers(1, &ubo_data.buffer_id);
	}
	
	// Clear the map.
	ubos_.clear();
}