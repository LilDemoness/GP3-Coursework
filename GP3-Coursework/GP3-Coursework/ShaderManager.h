#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include "Shader.h"

class ShaderManager
{
public:
	static ShaderManager& get_instance();

	std::shared_ptr<Shader> LoadShader(const std::string& tag, const std::string& shared_file_path);
	std::shared_ptr<Shader> GetShader(const std::string& tag);

	void SetActiveShader(const std::string& tag);
	std::shared_ptr<Shader> GetActiveShader();

	void Clear();

private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(ShaderManager& other) = delete;
	ShaderManager& operator=(const ShaderManager& other) = delete;

	std::string default_active_shader_tag;
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};