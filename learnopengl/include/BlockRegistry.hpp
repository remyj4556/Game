#ifndef BLOCKREGISTRY_HPP
#define BLOCKREGISTRY_HPP

#include <string>
#include <unordered_map>

#include "Vertex.hpp"
#include "BlockDefinition.hpp"

class BlockRegistry {
	private:
		BlockRegistry();
		std::unordered_map<int, BlockDefinition> block_defs;

	public:
		void addDefinition(const BlockDefinition& definition);
		const BlockDefinition& getDefinition(int id);

		// singleton instance
		static BlockRegistry& getInstance() {
			static BlockRegistry instance;
			return instance;
		}
};



#endif