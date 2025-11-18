#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "BlockRegistry.hpp"
#include "BlockDefinition.hpp"
#include "ModelLibrary.hpp"
#include "Vertex.hpp"
#include "TextureRegion.hpp"

// for testing
#include <iostream>

// a chunk should have an array of some fixed size representing every position in the chunk, then at each index we simply can have
// an int representing the block at that position.
// we store a Mesh as a private member variable. this will be assigned to whenever we call updateChunkMesh, as it can then be used
// multiple times for every draw call. we should only update, i.e., call updateChunkMesh, when a block is updated/broken/placed in the chunk

class Chunk {
	private:
		static constexpr int CHUNK_SIZE = 32;

	public:
		int positions[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		Mesh chunk_mesh;
		void updateChunkMesh();
		void printChunkVertices();
};

#endif
