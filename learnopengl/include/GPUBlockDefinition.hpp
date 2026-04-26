#ifndef GPUBLOCKDEFINITION_HPP
#define GPUBLOCKDEFINITION_HPP

#include <glm/glm.hpp>
#include <glm/fwd.hpp>

struct GPUBlockDefinition {
	glm::vec4 face_uv[6];
	glm::vec4 material;
};

#endif