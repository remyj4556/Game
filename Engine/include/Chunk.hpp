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

		// TODO: change the single chunk_mesh into opaque_mesh, transparent_mesh, cutout_mesh, etc.
		// these are rendered separately in the render loop, and within the meshes we can still
		// have different materials (sampled in the shader), for, say, reflectivity of metal vs. wood
		Mesh chunk_mesh;

		// dirty flag signals if the chunk has been altered
		bool dirty;

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

		void printChunkVertices();

	private:
		std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
		ChunkCoordinates chunk_position;
};

#endif
