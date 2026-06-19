#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "glm/fwd.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "Block.hpp"
#include <cstdint>

// TODO: this will need to be bit packed for maximum memory efficiency. OpenGL does not like 
struct Vertex {
	glm::vec3 position;
	block_id_type id = -1;
	uint8_t face = -1;
};

#endif