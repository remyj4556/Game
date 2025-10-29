#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"

// a chunk should have an array of some fixed size representing every position in the chunk, then at each index we simply can have
// an int representing the block at that position.
// we store a Mesh as a private member variable. this will be assigned to whenever we call updateChunkMesh, as it can then be used
// multiple times in a draw call. we should only update, i.e., call updateChunkMesh, when a block is updated/broken/placed in the chunk

class Chunk {
	private:
		static constexpr int CHUNK_SIZE = 32;
		Mesh chunk_mesh;

	public:
		int positions[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		void updateChunkMesh();
};

void Chunk::updateChunkMesh() {
	// iterate over every position in positions, and get the id of the block.
	// look in the block registry, and get its model type. use that to look in the
	// model library and get its actual vertex definitions. from the block registry
	// also get the texture offset for the block id, and add both of them to the local
	// vertex vector and texture vector respectively, and then create a mesh out of them.

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {

			}
		}
	}
}

#endif
