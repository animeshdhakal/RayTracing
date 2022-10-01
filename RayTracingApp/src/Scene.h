#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Sphere {
	glm::vec3 position{0.0f};
	float radius = 1.0f;
	glm::vec3 color{ 1.0f };
};

struct Scene {
	std::vector<Sphere> spheres;
};