#include "../include/Chunk.hpp"
#include "../include/Coordinates.hpp"
#include <string>

Chunk::Chunk() : blocks{ 0 }, state{ ChunkState::Unloaded } {}

const block_id_type Chunk::getBlock(CoordinateSystem::LocalCoordinates coordinates) const {
	return blocks[index(coordinates.x, coordinates.y, coordinates.z)];
}

void Chunk::setBlock(CoordinateSystem::LocalCoordinates coordinates, block_id_type block) {
	blocks[index(coordinates.x, coordinates.y, coordinates.z)] = block;
}

// QueuedToUnload separate from other states in that it can be advanced to from any other state
void Chunk::advanceChunkState() {
	switch (state) {
	case(ChunkState::Unloaded):
		state = ChunkState::QueuedToLoad; break;
	case(ChunkState::QueuedToLoad):
		state = ChunkState::Loaded; break;
	case(ChunkState::Loaded):
		state = ChunkState::QueuedToMesh; break;
	case(ChunkState::QueuedToMesh):
		state = ChunkState::Meshed; break;
	case(ChunkState::Meshed):
		state = ChunkState::Uploaded; break;
	}
}

std::string Chunk::chunkStateToString() const {
	switch (state) {
	case(ChunkState::Unloaded):
		return "Unloaded";
	case(ChunkState::QueuedToLoad):
		return "QueuedToLoad";
	case(ChunkState::Loaded):
		return "Loaded";
	case(ChunkState::QueuedToMesh):
		return "QueuedToMesh";
	case(ChunkState::Meshed):
		return "Meshed";
	case(ChunkState::QueuedToUnload):
		return "QueuedToUnload";
	}
}

std::string Chunk::chunkStateToString(ChunkState chunk_state) {
	switch (chunk_state) {
	case(ChunkState::Unloaded):
		return "Unloaded";
	case(ChunkState::QueuedToLoad):
		return "QueuedToLoad";
	case(ChunkState::Loaded):
		return "Loaded";
	case(ChunkState::QueuedToMesh):
		return "QueuedToMesh";
	case(ChunkState::Meshed):
		return "Meshed";
	case(ChunkState::QueuedToUnload):
		return "QueuedToUnload";
	}
}
