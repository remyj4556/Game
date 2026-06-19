#ifndef BLOCKREGISTRY_HPP
#define BLOCKREGISTRY_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "Block.hpp"
#include "BlockDefinition.hpp"
#include "TextureLibrary.hpp"
#include "GPUBlockDefinition.hpp"

class BlockRegistry {
	private:
		std::unordered_map<block_id_type, BlockDefinition> block_defs;

	public:
		BlockRegistry();
		void addDefinition(const BlockDefinition& definition);
		const BlockDefinition& getDefinition(block_id_type id) const;
		void populateDefinitions(const std::filesystem::path& block_defs_path, TextureLibrary& texture_library);
		std::vector<GPUBlockDefinition> buildGPUBlockDefinitions() const;
};

#endif