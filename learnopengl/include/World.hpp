#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkCoordinates.hpp"
#include "Camera.hpp"
#include <unordered_map>

class World {
	private:
		// TODO: ChunkMesher
		std::unordered_map<Coordinates, Chunk*, CoordinatesHash> coords_to_chunk;

	public:
		void genTestChunk();
		std::vector<Chunk*> getVisibleChunks(Camera &camera);
};

#endif