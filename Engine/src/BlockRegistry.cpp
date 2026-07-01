#include "../include/BlockRegistry.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/TextureLibrary.hpp"
#include "../include/json.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include "../include/BlockDefinition.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cassert>
#include <vector>

BlockRegistry::BlockRegistry() {}

void BlockRegistry::addDefinition(const BlockDefinition& definition) {
	block_defs[definition.id] = definition;
}

const BlockDefinition& BlockRegistry::getDefinition(block_id_type id) const {
	return block_defs.at(id);
}

void BlockRegistry::populateDefinitions(const std::filesystem::path& block_defs_path, TextureLibrary& texture_library) {
	std::ifstream file(block_defs_path);

	if (!file) {
		std::cerr << "Error: blocks.json could not be opened!\n";
		return;
	}

	nlohmann::json data = nlohmann::json::parse(file);
	
	for (nlohmann::json::iterator it = data["blocks"].begin(); it != data["blocks"].end(); ++it) {
		BlockDefinition definition;

		definition.id = (*it)["id"];

		definition.face_texture_id[0] = texture_library.getTextureID((*it)["back_texture"]);
		definition.face_texture_id[1] = texture_library.getTextureID((*it)["front_texture"]);
		definition.face_texture_id[2] = texture_library.getTextureID((*it)["left_texture"]);
		definition.face_texture_id[3] = texture_library.getTextureID((*it)["right_texture"]);
		definition.face_texture_id[4] = texture_library.getTextureID((*it)["bottom_texture"]);
		definition.face_texture_id[5] = texture_library.getTextureID((*it)["top_texture"]);

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

		gpu_def.face_texture_id_0_3.x = block_def.face_texture_id[0];
		gpu_def.face_texture_id_0_3.y = block_def.face_texture_id[1];
		gpu_def.face_texture_id_0_3.z = block_def.face_texture_id[2];
		gpu_def.face_texture_id_0_3.w = block_def.face_texture_id[3];
		gpu_def.face_texture_id_4_5.x = block_def.face_texture_id[4];
		gpu_def.face_texture_id_4_5.y = block_def.face_texture_id[5];

		gpu_def.material = glm::vec4(block_def.material.specular, block_def.material.shininess, 0.0f, 0.0f);

		definitions[block_id] = gpu_def;
	}

	assert(definitions.size() > 0);
	return definitions;
}