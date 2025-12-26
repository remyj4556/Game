#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include "TextureRegion.hpp"
#include <string>
#include <unordered_map>


// this enum class allows us to correctly reference the model shape
// in the model library, from the block registry 
enum class BlockModel {
	cube,
	stair,
	slab
	// etc.,
};

struct MaterialProperties {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	bool is_transparent;
};

// get a block model from a string
static const std::unordered_map<std::string, BlockModel> blockModelLookup = {
	{"cube", BlockModel::cube},
	{"stair", BlockModel::stair},
	{"slab", BlockModel::slab}
};

struct BlockDefinition {
	int id = -1;
	std::string display_name;
	BlockModel model = BlockModel::cube;
	TextureRegion textures[6];
	MaterialProperties material;
};

#endif