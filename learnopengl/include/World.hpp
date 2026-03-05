#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "ChunkCoordinates.hpp"
#include "Camera.hpp"
#include "BlockMeshingContext.hpp"
#include "FastNoiseLite.h"
#include <unordered_map>
#include <vector>

struct StreamTarget {
	glm::vec3 pos;
	int load_radius;
};

class World {
	private:
		Coordinates last_streamed_chunk_coord;
		int last_radius;

		// TODO: i think we *might* want a separate terrain generator class, right now i'll do this stuff here
		FastNoiseLite noise;

		ChunkMesher chunk_mesher;
		const BlockMeshingContext* context;
		// TODO: wherever we use Chunk* use unique_ptr, for mem leaks and whatnot
		std::unordered_map<Coordinates, Chunk*, CoordinatesHash> coords_to_chunk;
		void streamTerrain(StreamTarget target);
		Chunk* genChunk(Coordinates coordinates, int seed);

	public:
		World(BlockMeshingContext context);
		std::vector<Chunk*> getVisibleChunks(Camera &camera);
		void update(StreamTarget target);
};

#endif