#pragma once

#include "Shader.h"
#include "UBOManager.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>

class ShaderManager
{
public:
	static std::shared_ptr<Shader> load_shader(const std::string& tag, const std::string& vertex_shader_path, const std::string& geometry_shader_path, const std::string& fragment_shader_path);
	static std::shared_ptr<Shader> load_shader(const std::string& tag, const std::string& vertex_shader_path, const std::string& fragment_shader_path);
	static std::shared_ptr<Shader> load_shader(const std::string& tag, const std::string& shared_file_path);

	static std::shared_ptr<Shader> get_shader(const std::string& tag);

	static void set_active_shader(const std::string& tag);
	static std::shared_ptr<Shader> get_active_shader();

	static void clear();

	static void bind_all_shaders(const std::string& ubo_tag);

private:
	ShaderManager() = delete;
	~ShaderManager() = delete;
	ShaderManager(ShaderManager& other) = delete;
	ShaderManager& operator=(const ShaderManager& other) = delete;

	static std::string default_active_shader_tag_;
	static std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};