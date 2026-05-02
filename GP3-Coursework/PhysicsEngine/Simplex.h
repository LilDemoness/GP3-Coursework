#pragma once

#include <glm/glm.hpp>
#include <array>

// A simplex is a shape with 'n + 1' vertices (Where 'n' is the number of dimensions).
// Theoretically: Represents the simplest shape that can "select" a region of space
//		which potentially contains a test point (For GJK, the origin).
// Programatically: A wrapper around a std::array of positions.
struct Simplex
{
public:
	Simplex();

	Simplex& operator=(std::initializer_list<glm::vec3> list);

	void push_front(glm::vec3 point);

	inline glm::vec3& operator[](int i) { return points_[i]; };
	inline std::size_t size() const { return size_; }

	inline auto begin() const { return points_.begin(); }
	inline auto end() const { return points_.end() - (4 - size_); }

private:
	std::array<glm::vec3, 4> points_;
	int size_;
};

