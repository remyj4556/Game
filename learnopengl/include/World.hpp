#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "ChunkCoordinates.hpp"
#include "Camera.hpp"
#include <unordered_map>

class World {
	private:
		ChunkMesher chunk_mesher;
		std::unordered_map<Coordinates, Chunk*, CoordinatesHash> coords_to_chunk;

	public:
		void genTestChunk();
		std::vector<Chunk*> getVisibleChunks(Camera &camera);
};

#endif