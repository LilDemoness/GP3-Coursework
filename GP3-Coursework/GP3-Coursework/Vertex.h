#pragma once

struct Vertex
{
public:
	Vertex(const glm::vec3& pos, const glm::vec3 normal, const glm::vec2& texCoord)
	{
		this->position_ = pos;
		this->texture_coordinate_ = texCoord;
		this->normal_ = normal;
	}

	glm::vec3* get_pos() { return &position_; }
	glm::vec2* get_texture_coordinate() { return &texture_coordinate_; }
	glm::vec3* get_normal() { return &normal_; }

private:
	glm::vec3 position_;
	glm::vec2 texture_coordinate_;
	glm::vec3 normal_;
};