#include "../include/Chunk.hpp"
#include "../include/Coordinates.hpp"
#include "../include/Coordinates.hpp"
#include "../include/Mesh.hpp"

Chunk::Chunk() : blocks{0} {}

const block_id_type Chunk::getBlock(CoordinateSystem::LocalCoordinates coordinates) const {
	return blocks[index(coordinates.x, coordinates.y, coordinates.z)];
}

void Chunk::setBlock(CoordinateSystem::LocalCoordinates coordinates, block_id_type block) {
	blocks[index(coordinates.x, coordinates.y, coordinates.z)] = block;
}
