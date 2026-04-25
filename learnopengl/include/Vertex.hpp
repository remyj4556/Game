#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <Block.hpp>

struct Vertex {
	glm::vec3 position;
	block_id_type id = -1;
	uint8_t face = -1;
	glm::vec2 local_uv; 
};

#endif