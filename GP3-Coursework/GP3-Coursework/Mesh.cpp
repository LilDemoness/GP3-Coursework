#pragma once
#include "Mesh.h"

#include <iostream>


Mesh::Mesh()
{ }
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	: vertices_(vertices),
	  indices_(indices)
{
	InitMesh();
}
Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertex_buffer_object_);
	glDeleteBuffers(1, &element_buffer_object_);
	glDeleteVertexArrays(1, &vertex_array_object_);
}

void Mesh::InitMesh()
{
	// Create buffers.
	glGenVertexArrays(1, &vertex_array_object_);
	glGenBuffers(1, &vertex_buffer_object_);
	glGenBuffers(1, &element_buffer_object_);


	// Push into a Vector (Convert from AOS to SOA).
	std::vector<float> interleaved_data;
	for (size_t i = 0; i < vertices_.size(); ++i)
	{
		// Positions.
		interleaved_data.push_back(vertices_[i].position.x);
		interleaved_data.push_back(vertices_[i].position.y);
		interleaved_data.push_back(vertices_[i].position.z);

		// Normals.
		interleaved_data.push_back(vertices_[i].normal.x);
		interleaved_data.push_back(vertices_[i].normal.y);
		interleaved_data.push_back(vertices_[i].normal.z);
		
		// Texture Coordinates.
		interleaved_data.push_back(vertices_[i].texture_coordinate.x);
		interleaved_data.push_back(vertices_[i].texture_coordinate.y);
	}


	glBindVertexArray(vertex_array_object_);

	// Upload interweaved vertex data.
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, interleaved_data.size() * sizeof(float), interleaved_data.data(), GL_STATIC_DRAW);

	// Index data to EBO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

	// Dynamic Vertex Attributes.
	//size_t stride = VertexAttribute::get_total_stride();
	/*std::cout << stride << std::endl;
	for (const VertexAttribute& attribute : VertexAttribute::VertexAttributes)
	{
		glEnableVertexAttribArray(attribute.index);
		glVertexAttribPointer(attribute.index, attribute.size, attribute.size, attribute.is_normalized, stride, (void*)attribute.offset);
	}*/
	size_t stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);
	// Position.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// Normals.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

	// Texcoords.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));


	glBindVertexArray(0);
}


void Mesh::Draw() const
{
	glBindVertexArray(vertex_array_object_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	/*glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	
	for each (VertexAttribute attribute in VertexAttribute::VertexAttributes)
		glEnableVertexAttribArray(attribute.index);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

	for each (VertexAttribute attribute in VertexAttribute::VertexAttributes)
		glDisableVertexAttribArray(attribute.index);*/
}