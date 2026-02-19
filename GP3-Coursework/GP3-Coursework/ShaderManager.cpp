#pragma once

#include "ShaderManager.h"

ShaderManager& ShaderManager::get_instance()
{
	static ShaderManager instance;
	return instance;
}
ShaderManager::ShaderManager()
{ }
ShaderManager::~ShaderManager()
{ }


std::shared_ptr<Shader> ShaderManager::LoadShader(const std::string& tag, const std::string& shared_file_path)
{
	if (shaders_.find(tag) != shaders_.end())
	{
		std::cerr << "Already Loaded Shader: " << shared_file_path << std::endl;
		return shaders_[tag];
	}
	if (default_active_shader_tag.empty())
		default_active_shader_tag = tag;

	std::shared_ptr<Shader> shader = std::make_shared<Shader>(shared_file_path);	// Declare the shared pointer on the heap.
	shaders_[tag] = shader;
	return shader;
}
std::shared_ptr<Shader> ShaderManager::GetShader(const std::string& tag)
{
	if (shaders_.find(tag) == shaders_.end())
	{
		std::cerr << "No Shader found for Tag: " << tag << std::endl;
		return nullptr;
	}

	return shaders_[tag];
}


void ShaderManager::SetActiveShader(const std::string& tag)
{
	if (tag.empty())
	{
		std::cerr << "Cannot set Active Shader to an empty tag." << std::endl;
		return;
	}
	if (shaders_.find(tag) == shaders_.end())
	{
		std::cerr << "No Shader found for Tag: " << tag << std::endl;
		return;
	}

	default_active_shader_tag = tag;
}
std::shared_ptr<Shader> ShaderManager::GetActiveShader()
{
	return shaders_[default_active_shader_tag];
}


void ShaderManager::Clear()
{
	shaders_.clear();
}


void ShaderManager::BindAllShaders(const std::string& ubo_tag)
{
	// Loop through all Shaders and bind our UBO.
	for (auto const& [id, shader] : shaders_)
	{
		UBOManager::get_instance().BindUBOToShader(ubo_tag, shader->get_id(), ubo_tag);
	}
}