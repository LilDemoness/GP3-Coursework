#pragma once

#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include <iostream>

static const std::string kMatricesTag = "Matrices";
class UBOManager
{
public:
	static UBOManager& get_instance();

	void CreateUBO(const std::string& tag, const size_t size, const GLuint binding_point);
	void UpdateUBOData(const std::string& tag, const size_t offset, const void* data, const size_t data_size);
	void BindUBOToShader(const std::string& ubo_tag, const GLuint shader_id, const std::string& ubo_name);

	void Clear();	// Cleanup all UBOs.



private:
	UBOManager();
	~UBOManager();
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

	std::unordered_map<std::string, UBOData> ubos_;
};