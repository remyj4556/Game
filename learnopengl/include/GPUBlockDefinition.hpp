#ifndef GPUBLOCKDEFINITION_HPP
#define GPUBLOCKDEFINITION_HPP

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

struct GPUBlockDefinition {
	glm::ivec4 face_texture_id_0_3;
	glm::ivec2 face_texture_id_4_5;
	glm::vec2 padding; // so it matches GLSL struct layout
	glm::vec4 material;
};

#endif