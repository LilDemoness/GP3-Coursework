#pragma once

#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include <iostream>

static const std::string kMatricesTag = "CameraMatrices";
class UBOManager
{
public:
	static void create_ubo(const std::string& tag, const size_t size, const GLuint binding_point);
	static void create_ubo_data(const std::string& tag, const size_t offset, const void* data, const size_t data_size);
	static void bind_ubo_to_shader(const std::string& ubo_tag, const GLuint shader_id, const std::string& ubo_name);

	static void clear();	// Cleanup all UBOs.



private:
	UBOManager() = delete;
	~UBOManager() = delete;
	UBOManager(const UBOManager&) = delete;
	UBOManager& operator=(const UBOManager&) = delete;

	struct UBOData
	{
		GLuint buffer_id;
		size_t buffer_size;
		GLuint binding_point;

		UBOData() = default;
		UBOData(const GLuint buffer_id, const size_t buffer_size, const GLuint binding_point)
		{
			this->buffer_id = buffer_id;
			this->buffer_size = buffer_size;
			this->binding_point = binding_point;
		}
	};

	static std::unordered_map<std::string, UBOData> ubos_;
};