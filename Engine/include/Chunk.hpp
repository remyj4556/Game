#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "Coordinates.hpp"
#include "BlockDefinition.hpp"
#include <array>

class Chunk {
	public:
		Chunk();
		static constexpr int CHUNK_SIZE = 32;

		// block access
		const block_id_type getBlock(CoordinateSystem::LocalCoordinates coordinates) const;
		void setBlock(CoordinateSystem::LocalCoordinates coordinates, block_id_type block);
		inline int index(int x, int y, int z) const {
			return x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE));
		}
		inline block_id_type getBlockFast(int x, int y, int z) const {
			return blocks[x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE))];
		}

	private:
		std::array<block_id_type, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
};

#endif
