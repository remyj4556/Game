#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <Block.hpp>

// this will need to be bit packed for maximum memory efficiency. OpenGL does not like 
struct Vertex {
	glm::vec3 position;
	block_id_type id = -1;
	uint8_t face = -1;
	glm::vec2 local_uv; 
};

#endif