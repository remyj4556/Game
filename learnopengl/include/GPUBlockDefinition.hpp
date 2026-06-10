#ifndef GPUBLOCKDEFINITION_HPP
#define GPUBLOCKDEFINITION_HPP

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

struct GPUBlockDefinition {
	glm::vec4 face_uv[6];
	glm::vec4 material;
};

#endif