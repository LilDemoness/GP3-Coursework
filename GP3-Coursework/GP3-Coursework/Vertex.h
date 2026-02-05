#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_coordinate;


	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture_coordinate) :
		position(position),
		normal(normal),
		texture_coordinate(texture_coordinate)
	{ }
};