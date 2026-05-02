#include "pch.h"
#include "Simplex.h"


Simplex::Simplex() :
	size_(0)
{}

Simplex& Simplex::operator=(std::initializer_list<glm::vec3> list)
{
	size_ = 0;

	for (const glm::vec3& point : list)
		points_[size_++] = point;
	
	return *this;
}

void Simplex::push_front(glm::vec3 point)
{
	points_ = { point, points_[0], points_[1], points_[2] };
	size_ = std::min(size_ + 1, 4);
}