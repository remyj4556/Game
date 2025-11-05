#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include "Texture.hpp"
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
	// here we want to store the OFFSET (or coords somehow) of the required texture in the texture atlas
	// TEST: now we simply store the cobble texture as is
	Texture texture = Texture("C:/Users/remyj/source/repos/Game2/learnopengl/textures/cobblestone.png", GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE);
};

#endif