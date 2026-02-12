#pragma once

#include "Transform.h"
#include "Camera.h"
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>

class Shader
{
public:
	Shader(const std::string& shared_file_path);
    ~Shader();

	void Bind();
	void Update(const Transform& transform, const Camera& camera);

	bool TryLoadShader(const std::string& file_name, std::string& shader);
	void CheckShaderError(GLuint shader, GLuint flag, bool is_program, const std::string& error_message);
	GLuint CreateShader(const std::string& identifier, const std::string& text, unsigned int type);

protected:
private:
	static const unsigned int kNumShaders = 3;	// Max number of shader files per shaders.

	enum
	{
		kMVPMatrix,
		kModelMatrix,
		kViewMatrix,

		kNormalMatrix,
		kIVPMatrix,
		kIPMatrix,

		kNumUniforms,
	};

	void InitialiseShaders(const std::string& shader_name, const unsigned int& shader_count);

	GLuint shader_id_;	// Track the shader program.
	GLuint shaders_[kNumShaders];	// Array of shaders.
	GLuint uniforms_[kNumUniforms];	// Number of uniform variables.


public:
	// ------------------------------------------------------------------------
	void set_bool(const std::string& name, bool value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform1i(glGetUniformLocation(shader_id_, name.c_str()), (int)value);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_int(const std::string& name, int value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform1i(glGetUniformLocation(shader_id_, name.c_str()), value);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_float(const std::string& name, float value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform1f(glGetUniformLocation(shader_id_, name.c_str()), value);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_vec2(const std::string& name, const glm::vec2& value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform2fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec2(const std::string& name, float x, float y, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform2f(glGetUniformLocation(shader_id_, name.c_str()), x, y);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_vec3(const std::string& name, const glm::vec3& value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform3fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec3(const std::string& name, float x, float y, float z, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform3f(glGetUniformLocation(shader_id_, name.c_str()), x, y, z);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_vec4(const std::string& name, const glm::vec4& value, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniform4fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec4(const std::string& name, float x, float y, float z, float w, const bool& ignore_errors = false)
	{
		glUseProgram(shader_id_);
		glUniform4f(glGetUniformLocation(shader_id_, name.c_str()), x, y, z, w);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat2(const std::string& name, const glm::mat2& mat, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniformMatrix2fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat3(const std::string& name, const glm::mat3& mat, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniformMatrix3fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat4(const std::string& name, const glm::mat4& mat, const bool& ignore_errors = false) const
	{
		glUseProgram(shader_id_);
		glUniformMatrix4fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1) && !ignore_errors)
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
};