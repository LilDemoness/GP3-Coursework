#pragma once

#include "ShaderManager.h"

std::string ShaderManager::default_active_shader_tag_ = "";
std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaders_;


std::shared_ptr<Shader> ShaderManager::load_shader(const std::string& tag, const std::string& vertex_shader_path, const std::string& geometry_shader_path, const std::string& fragment_shader_path)
{
	if (shaders_.find(tag) != shaders_.end())
	{
		std::cerr << "Already Loaded Shader: " << vertex_shader_path.substr(0, vertex_shader_path.size() - 5) << std::endl;
		return shaders_[tag];
	}
	if (default_active_shader_tag_.empty())
		default_active_shader_tag_ = tag;

	std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertex_shader_path, geometry_shader_path, fragment_shader_path);	// Declare the shared pointer on the heap.
	shaders_[tag] = shader;
	return shader;
}
std::shared_ptr<Shader> ShaderManager::load_shader(const std::string& tag, const std::string& vertex_shader_path, const std::string& fragment_shader_path) { return ShaderManager::load_shader(tag, vertex_shader_path, "", fragment_shader_path); }
std::shared_ptr<Shader> ShaderManager::load_shader(const std::string& tag, const std::string& shared_file_path) { return ShaderManager::load_shader(tag, shared_file_path + ".vert", shared_file_path + ".geom", shared_file_path + ".frag"); }
std::shared_ptr<Shader> ShaderManager::get_shader(const std::string& tag)
{
	if (shaders_.find(tag) == shaders_.end())
	{
		std::cerr << "No Shader found for Tag: " << tag << std::endl;
		return nullptr;
	}

	return shaders_[tag];
}


void ShaderManager::set_active_shader(const std::string& tag)
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

	default_active_shader_tag_ = tag;
}
std::shared_ptr<Shader> ShaderManager::get_active_shader()
{
	return shaders_[default_active_shader_tag_];
}


void ShaderManager::clear()
{
	shaders_.clear();
}


void ShaderManager::bind_all_shaders(const std::string& ubo_tag)
{
	// Loop through all Shaders and bind our UBO.
	for (auto const& [id, shader] : shaders_)
	{
		UBOManager::bind_ubo_to_shader(ubo_tag, shader->get_id(), ubo_tag);
	}
}