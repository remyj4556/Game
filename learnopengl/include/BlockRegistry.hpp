#ifndef BLOCKREGISTRY_HPP
#define BLOCKREGISTRY_HPP

#include <string>
#include <unordered_map>
#include <fstream>

#include "BlockDefinition.hpp"
#include "json.hpp"
#include "TextureAtlas.hpp"

class BlockRegistry {
	private:
		BlockRegistry();
		std::unordered_map<int, BlockDefinition> block_defs;

	public:
		void addDefinition(const BlockDefinition& definition);
		const BlockDefinition& getDefinition(int id);
		void populateDefinitions(const std::string& path, TextureAtlas& atlas);

		// singleton instance
		static BlockRegistry& getInstance() {
			static BlockRegistry instance;
			return instance;
		}
};



#endif