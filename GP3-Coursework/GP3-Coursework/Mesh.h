#pragma once

#include <vector>
#include "VertexAttribute.h"
#include "Vertex.h"

class Mesh
{
public:
	Mesh();
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();


	void Draw() const;


private:
	void InitMesh();

	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;

	GLuint vertex_array_object_;
	GLuint vertex_buffer_object_;
	GLuint element_buffer_object_;
};

