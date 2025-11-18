#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include "Texture.hpp"
#include "TextureRegion.hpp"
#include <string>


// this enum class allows us to correctly reference the model shape
// in the model library, from the block registry 
enum class BlockModel {
	cube,
	// stair,
	// slab, 
	// etc.,
};

struct BlockDefinition {
	int id = -1;
	std::string display_name;
	BlockModel model = BlockModel::cube;
	TextureRegion textures[6];
};

#endif