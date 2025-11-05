#include "../include/BlockRegistry.hpp"

BlockRegistry::BlockRegistry() {}

void BlockRegistry::addDefinition(const BlockDefinition& definition) {
	block_defs[definition.id] = definition;
}

const BlockDefinition& BlockRegistry::getDefinition(int id) {
	return block_defs[id];
}