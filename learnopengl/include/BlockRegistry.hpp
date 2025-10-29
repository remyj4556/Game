#ifndef BLOCKREGISTRY_HPP
#define BLOCKREGISTRY_HPP

#include <string>
#include <vector>

#include "Vertex.hpp"

// perhaps define this in another file, we need this ordering to match that in the ModelLibrary
// somehow, such that we can, given a BlockModel enum, access its corresponding geometry from 
// the ModelLibrary class
enum class BlockModel {
	cube,
};

struct BlockDefinition {
	int id;
	std::string display_name;
	BlockModel model;
	// probably want to store the pointer to the texture here, or some reference to it
};

class BlockRegistry {
	public:
		BlockRegistry();
		BlockRegistry(std::vector<BlockDefinition> defs);
		void addDefinition(BlockDefinition definition);
		std::vector<BlockDefinition> block_defs;
};

BlockRegistry::BlockRegistry() {}

BlockRegistry::BlockRegistry(std::vector<BlockDefinition> defs) : block_defs(defs) {}

void BlockRegistry::addDefinition(BlockDefinition definition) {
	block_defs.push_back(definition);
}



#endif