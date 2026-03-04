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
		std::unordered_map<int, BlockDefinition> block_defs;

	public:
		BlockRegistry();
		void addDefinition(const BlockDefinition& definition);
		const BlockDefinition& getDefinition(int id) const;
		void populateDefinitions(const std::string& path, TextureAtlas& atlas);
};



#endif