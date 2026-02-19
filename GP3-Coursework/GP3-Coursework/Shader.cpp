#pragma once
#include "Shader.h"


Shader::Shader(const std::string& shared_file_path)
{
	// Create our shader programe (OpenGL Saves this as a Reference Number)
	shader_id_ = glCreateProgram();


	// Load our shaders (Vertex + (Geometry if it exists) + Fragment).
	int loaded_shaders = 0;
	std::string loaded_shader;

	// Vertex.
	if (TryLoadShader(shared_file_path + ".vert", loaded_shader))
		shaders_[loaded_shaders++] = CreateShader(shared_file_path + ".vert", loaded_shader, GL_VERTEX_SHADER);
	else
		std::cout << "Failed to load Vertex Shader" << std::endl;	// Vertex shader failed to load.

	// Geometry (Optional).
	if (TryLoadShader(shared_file_path + ".geom", loaded_shader))
		shaders_[loaded_shaders++] = CreateShader(shared_file_path + ".geom", loaded_shader, GL_GEOMETRY_SHADER);

	// Vertex.
	if (TryLoadShader(shared_file_path + ".frag", loaded_shader))
		shaders_[loaded_shaders++] = CreateShader(shared_file_path + ".frag", loaded_shader, GL_FRAGMENT_SHADER);
	else
		std::cout << "Failed to load Fragment Shader" << std::endl;	// Fragment shader failed to load.


	this->InitialiseShaders(shared_file_path, loaded_shaders);
}
Shader::~Shader()
{
	for (size_t i = 0; i < kNumShaders; ++i)
	{
		glDetachShader(shader_id_, shaders_[i]);	// Detach the shader from the programme.
		glDeleteShader(shaders_[i]);	// Delete the shader.
	}

	glDeleteProgram(shader_id_);	// Delete the program.
}


void Shader::InitialiseShaders(const std::string& shader_name, const unsigned int& shader_count)
{
	// All all our shaders to the shader program's shaders.
	for (size_t i = 0; i < shader_count; ++i)
	{
		glAttachShader(shader_id_, shaders_[i]);
	}

	glLinkProgram(shader_id_);	// Creates .exes that will run on the GPU shaders.

	// Check for Errors.
	CheckShaderError(shader_id_, GL_LINK_STATUS, true, "Error: Shader " + shader_name + " linking failed");	// .exe creation error.


	// Associate the location of our uniform variables within the programmes.
	uniforms_[kMVPMatrix] = glGetUniformLocation(shader_id_, "transform");
	uniforms_[kModelMatrix] = glGetUniformLocation(shader_id_, "modelMatrix");
	uniforms_[kViewMatrix] = glGetUniformLocation(shader_id_, "viewMatrix");
	uniforms_[kNormalMatrix] = glGetUniformLocation(shader_id_, "normalMatrix");
	uniforms_[kIVPMatrix] = glGetUniformLocation(shader_id_, "inverseViewProjectionMatrix");
	uniforms_[kIPMatrix] = glGetUniformLocation(shader_id_, "inverseProjectionMatrix");
}


void Shader::Bind()
{
	glValidateProgram(shader_id_);	// Check the entire programme is valid.
	CheckShaderError(shader_id_, GL_VALIDATE_STATUS, true, "Error: Shader not valid");	// Shader Validation Error.

	glUseProgram(shader_id_);	// Installs the program object specified by the program as part of the rendering state.
}
void Shader::UpdateMatricesUBO(const Transform& transform, const Camera& camera)
{
	const size_t kMat4Size = sizeof(glm::mat4);

	UBOManager::get_instance().UpdateUBOData(kMatricesTag, 0, glm::value_ptr(transform.get_model()), kMat4Size);
	UBOManager::get_instance().UpdateUBOData(kMatricesTag, kMat4Size, glm::value_ptr(camera.get_view()), kMat4Size);
	UBOManager::get_instance().UpdateUBOData(kMatricesTag, kMat4Size * 2, glm::value_ptr(camera.get_projection()), kMat4Size);
}


GLuint Shader::CreateShader(const std::string& identifier, const std::string& text, unsigned int type)
{
	GLuint shader = glCreateShader(type);	// Create the shader based on the specified type.

	if (shader == 0)	// shader == 0 if none was created.
		std::cerr << "Error type creation failed " << type << std::endl;

	const GLchar* string_source[1] = { text.c_str() };	// Convert strings into a list of c-strings.
	GLint lengths[1] = { text.length() };

	glShaderSource(shader, 1, string_source, lengths);	// Send the source code to OpenGL.
	glCompileShader(shader);	// Get OpenGL to compile the shader code.

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");	// Check for compile error

	return shader;
}
bool Shader::TryLoadShader(const std::string& file_name, std::string& shader)
{
	std::ifstream file;
	file.open((file_name).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + '\n');
		}
	}
	else
	{
		return false;
	}

	shader = output;
	return true;
}
void Shader::CheckShaderError(GLuint shader, GLuint flag, bool is_program, const std::string& error_message)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (is_program)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (is_program)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << is_program << ": '" << error << "'" << std::endl;
	}
}