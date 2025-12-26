#ifndef GEOMETRYVERTEX_HPP
#define GEOMETRYVERTEX_HPP

#include <glm/glm.hpp>

struct GeometryVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

#endif