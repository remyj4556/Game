#include "../include/BlockRegistry.hpp"
#include <iostream>

BlockRegistry::BlockRegistry() {}

void BlockRegistry::addDefinition(const BlockDefinition& definition) {
	block_defs[definition.id] = definition;
}

const BlockDefinition& BlockRegistry::getDefinition(int id) {
	return block_defs[id];
}

void BlockRegistry::populateDefinitions(const std::string& path, TextureAtlas & atlas) {
	std::ifstream file(path);
	nlohmann::json data = nlohmann::json::parse(file);
	
	for (nlohmann::json::iterator it = data["blocks"].begin(); it != data["blocks"].end(); ++it) {
		BlockDefinition definition;

		definition.display_name = (*it)["display_name"];
		definition.id = (*it)["id"];
		
		try {
			definition.model = blockModelLookup.at((*it)["model"].get<std::string>());
		}
		catch (const std::out_of_range& oor) {
			std::cerr << "error populating definition for block: " << (*it)["display_name"] << "\n"
				      << " - block model: " << (*it)["model"].get<std::string>() << " does not exist\n";
		}
		
		definition.textures[0] = atlas.getTextureRegion((*it)["back_texture"]);
		definition.textures[1] = atlas.getTextureRegion((*it)["front_texture"]);
		definition.textures[2] = atlas.getTextureRegion((*it)["left_texture"]);
		definition.textures[3] = atlas.getTextureRegion((*it)["right_texture"]);
		definition.textures[4] = atlas.getTextureRegion((*it)["bottom_texture"]);
		definition.textures[5] = atlas.getTextureRegion((*it)["top_texture"]);
		
		addDefinition(definition);
	}
}