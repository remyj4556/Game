#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/Mesh.hpp"

Chunk::Chunk() :chunk_position({0,0,0}), chunk_mesh(Mesh()), dirty(true) {
}

Chunk::Chunk(Coordinates coordinates) : chunk_position(coordinates), chunk_mesh(Mesh()), dirty(true) {
}

void Chunk::printChunkVertices() {
	chunk_mesh.printInfo();
}

int Chunk::getChunkSize() {
	return CHUNK_SIZE;
}