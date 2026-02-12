#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include <iostream>
#include <vector>
#include "Vertex.h"

class Mesh {
public:
	Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices);
	Mesh(const std::string& file_name);
	~Mesh();


	// Creates a simple triangle mesh.
	static Mesh CreateTriangleMesh();


	void Draw();

private:
	//Mesh(Mesh& other) = delete;

	void InitModel(const IndexedModel& model);

	GLuint vertex_array_object_ = 0;	// Vertex Array Object.
	GLuint vertex_buffer_object_ = 0;	// Interleaved Vertex Buffer Object.
	GLuint element_buffer_object_ = 0;  // Index Buffer Object.
	unsigned int draw_count_ = 0;		// Number of Indices.
};
