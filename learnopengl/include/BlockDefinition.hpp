#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include "TextureRegion.hpp"
#include "Block.hpp"
#include <string>
#include <unordered_map>


// this enum class allows us to correctly reference the model shape
// in the model library, from the block registry 
enum class BlockModel {
	cube,
	stair,
	slab,
	// etc.,
};
constexpr int BlockModelCount = 3;

struct MaterialProperties {
	glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse = { 0.0f, 0.0f, 0.0f };
	glm::vec3 specular = { 0.0f, 0.0f, 0.0f };
	float shininess = 1.0f;
	bool is_transparent = false;
};

// get a block model from a string
static const std::unordered_map<std::string, BlockModel> blockModelLookup = {
	{"cube", BlockModel::cube},
	{"stair", BlockModel::stair},
	{"slab", BlockModel::slab}
};

struct BlockDefinition {
	block_id_type id;
	std::string display_name;
	BlockModel model = BlockModel::cube;
	TextureRegion textures[6];
	MaterialProperties material;
};

#endif