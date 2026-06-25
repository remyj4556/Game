#include "../include/Chunk.hpp"
#include "../include/Coordinates.hpp"
#include "../include/Coordinates.hpp"
#include "../include/Mesh.hpp"

Chunk::Chunk() 
	: chunk_position({0,0,0})
	, blocks{0}
{}

Chunk::Chunk(CoordinateSystem::ChunkCoordinates coordinates) 
	: chunk_position(coordinates)
	, blocks{0}
{}

const Block Chunk::getBlock(CoordinateSystem::LocalCoordinates coordinates) const {
	return blocks[index(coordinates.x, coordinates.y, coordinates.z)];
}

void Chunk::setBlock(CoordinateSystem::LocalCoordinates coordinates, Block block) {
	blocks[index(coordinates.x, coordinates.y, coordinates.z)] = block;
}

const CoordinateSystem::ChunkCoordinates Chunk::getChunkPosition() const {
	return chunk_position;
}