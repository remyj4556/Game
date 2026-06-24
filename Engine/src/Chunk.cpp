#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/Mesh.hpp"

Chunk::Chunk() 
	: chunk_position({0,0,0})
	, blocks{0}
{}

Chunk::Chunk(ChunkCoordinates coordinates) 
	: chunk_position(coordinates)
	, blocks{0}
{}

const Block Chunk::getBlock(LocalCoordinates coordinates) const {
	return blocks[index(coordinates.x, coordinates.y, coordinates.z)];
}

void Chunk::setBlock(LocalCoordinates coordinates, Block block) {
	blocks[index(coordinates.x, coordinates.y, coordinates.z)] = block;
}

const ChunkCoordinates Chunk::getChunkPosition() const {
	return chunk_position;
}