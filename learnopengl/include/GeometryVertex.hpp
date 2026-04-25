#ifndef GEOMETRYVERTEX_HPP
#define GEOMETRYVERTEX_HPP

#include <glm/glm.hpp>
#include <Block.hpp>

struct GeometryVertex {
	glm::vec3 position;
	block_id_type id;
	uint8_t face;

	// unnecessary to store per vertex
	//glm::vec3 normal;
	glm::vec2 tex_coords; // TODO: this is LOCAL UV's, i.e., ONLY 0-1 (after change)
};

#endif