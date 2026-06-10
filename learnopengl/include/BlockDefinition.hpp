#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include "Block.hpp"
#include "ModelLibrary.hpp"
#include <string>
#include <unordered_map>
#include "glm/fwd.hpp"
#include "glm/vec4.hpp"

struct MaterialProperties {
	float specular = 0.0f;
	float shininess = 0.0f;
};

// get a block model from a string
static const std::unordered_map<std::string, ModelTypes> blockModelLookup = {
	{"cube", ModelTypes::cube},
	{"stair", ModelTypes::stair},
	{"slab", ModelTypes::slab}
};

struct BlockDefinition {
	block_id_type id;
	bool is_transparent = false;
	ModelTypes model = ModelTypes::cube;
	glm::vec4 face_uv[6];
	MaterialProperties material;
};

#endif