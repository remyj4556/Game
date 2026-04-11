#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/Mesh.hpp"

Chunk::Chunk() :chunk_position({0,0,0}), chunk_mesh(Mesh()), dirty(true) {
}

Chunk::Chunk(ChunkCoordinates coordinates) : chunk_position(coordinates), chunk_mesh(Mesh()), dirty(true) {
}

void Chunk::printChunkVertices() {
	chunk_mesh.printInfo();
}

int Chunk::getChunkSize() {
	return CHUNK_SIZE;
}

const Block Chunk::getBlock(LocalCoordinates coordinates) const {
	return positions[coordinates.x][coordinates.y][coordinates.z];
}

void Chunk::setBlock(LocalCoordinates coordinates, Block block) {
	positions[coordinates.x][coordinates.y][coordinates.z] = block;
}

const ChunkCoordinates Chunk::getChunkPosition() const {
	return chunk_position;
}