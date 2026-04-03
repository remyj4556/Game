#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "ChunkCoordinates.hpp"
#include "Block.hpp"


class Chunk {
	private:
		static constexpr int CHUNK_SIZE = 32;

	public:
		Chunk();
		Chunk(Coordinates coordinates);

		Block positions[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = { 0 };  // TODO: change to single array, or packed int

		Coordinates chunk_position;

		// TODO: change the single chunk_mesh into opaque_mesh, transparent_mesh, cutout_mesh, etc.
		// these are rendered separately in the render loop, and within the meshes we can still
		// have different materials (sampled in the shader), for, say, reflectivity of metal vs. wood
		// TODO: eventually don't store mesh at all, put them in map keyed by coordinates
		Mesh chunk_mesh;

		// dirty flag signals if the chunk has been altered
		bool dirty;

		void printChunkVertices();
		static int getChunkSize();
};

#endif
