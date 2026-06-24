#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "ChunkCoordinates.hpp"
#include "LocalCoordinates.hpp"
#include "Block.hpp"

#include <cstdint>
#include <array>


class Chunk {
	public:
		Chunk();
		Chunk(ChunkCoordinates coordinates);
		static constexpr int CHUNK_SIZE = 32;

		// block access
		const Block getBlock(LocalCoordinates coordinates) const;
		void setBlock(LocalCoordinates coordinates, Block block);
		inline int index(int x, int y, int z) const {
			return x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE));
		}
		inline Block getBlockFast(int x, int y, int z) const {
			return blocks[x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE))];
		}

		const ChunkCoordinates getChunkPosition() const;

	private:
		std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
		ChunkCoordinates chunk_position;
};

#endif
