#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "ChunkCoordinates.hpp"
#include "Block.hpp"

#include <cstdint>

struct LocalCoordinates {
	uint8_t x;
	uint8_t y;
	uint8_t z;
};

class Chunk {
	private:
		static constexpr int CHUNK_SIZE = 32;
		Block positions[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 };  // TODO: change to single array, or packed int
		ChunkCoordinates chunk_position;

	public:
		Chunk();
		Chunk(ChunkCoordinates coordinates);

		// TODO: change the single chunk_mesh into opaque_mesh, transparent_mesh, cutout_mesh, etc.
		// these are rendered separately in the render loop, and within the meshes we can still
		// have different materials (sampled in the shader), for, say, reflectivity of metal vs. wood
		// TODO: eventually don't store mesh at all, put them in map keyed by coordinates
		Mesh chunk_mesh;

		// dirty flag signals if the chunk has been altered
		bool dirty;

		// block access
		const Block getBlock(LocalCoordinates coordinates) const;
		void setBlock(LocalCoordinates coordinates, Block block);

		const ChunkCoordinates getChunkPosition() const;

		void printChunkVertices();
		static int getChunkSize();
};

#endif
