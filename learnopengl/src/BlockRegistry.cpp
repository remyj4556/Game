#include "../include/BlockRegistry.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/TextureAtlas.hpp"
#include "../include/json.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include "../include/Block.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>

BlockRegistry::BlockRegistry() {}

void BlockRegistry::addDefinition(const BlockDefinition& definition) {
	block_defs[definition.id] = definition;
}

const BlockDefinition& BlockRegistry::getDefinition(block_id_type id) const {
	return block_defs.at(id);
}

void BlockRegistry::populateDefinitions(const std::string& path, TextureAtlas & atlas) {
	std::ifstream file(path);
	nlohmann::json data = nlohmann::json::parse(file);
	
	for (nlohmann::json::iterator it = data["blocks"].begin(); it != data["blocks"].end(); ++it) {
		BlockDefinition definition;

		definition.id = (*it)["id"];

		try {
			definition.model = blockModelLookup.at((*it)["model"].get<std::string>());
		}
		catch (const std::out_of_range& oor) {
			std::cerr << "error populating definition for block: " << (*it)["display_name"] << "\n"
				<< " - block model: " << (*it)["model"].get<std::string>() << " does not exist\n";
		}

		definition.face_uv[0] = atlas.getTextureRegion((*it)["back_texture"]);
		definition.face_uv[1] = atlas.getTextureRegion((*it)["front_texture"]);
		definition.face_uv[2] = atlas.getTextureRegion((*it)["left_texture"]);
		definition.face_uv[3] = atlas.getTextureRegion((*it)["right_texture"]);
		definition.face_uv[4] = atlas.getTextureRegion((*it)["bottom_texture"]);
		definition.face_uv[5] = atlas.getTextureRegion((*it)["top_texture"]);

		float specular  = (*it)["material"]["specular"];
		float shininess  = (*it)["material"]["shininess"];

		definition.material.specular = specular;
		definition.material.shininess = shininess;
		definition.is_transparent = (*it)["material"]["transparent"];
		
		addDefinition(definition);
	}
}

std::vector<GPUBlockDefinition> BlockRegistry::buildGPUBlockDefinitions() const {
	std::vector<GPUBlockDefinition> definitions(block_defs.size());

	for (const auto& [block_id, block_def] : block_defs) {
		GPUBlockDefinition gpu_def{};

		for (int i = 0; i < 6; ++i) {
			gpu_def.face_uv[i] = block_def.face_uv[i];
		}

		gpu_def.material = glm::vec4(block_def.material.specular, block_def.material.shininess, 0.0f, 0.0f);

		definitions[block_id] = gpu_def;
	}

	return definitions;
}