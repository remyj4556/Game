#include "../include/Chunk.hpp"


void Chunk::printChunkVertices() {
	chunk_mesh.printInfo();
}

int Chunk::getChunkSize() {
	return CHUNK_SIZE;
}