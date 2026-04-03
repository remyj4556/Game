#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "ChunkCoordinates.hpp"
#include "WorldCoordinates.hpp"
#include "BlockMeshingContext.hpp"
#include "FastNoiseLite.h"
#include "Block.hpp"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>

struct StreamTarget {
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	int load_radius = 0;
};

struct ChunkGenRequest {
	Coordinates coordinates;
	float dist_to_target;

	bool operator>(const ChunkGenRequest& other) const {
		return dist_to_target > other.dist_to_target;
	}
};

class World {
	private:

		Coordinates last_streamed_chunk_coord;
		int last_radius;

		// TODO: i think we *might* want a separate terrain generator class, right now i'll do this stuff here
		FastNoiseLite noise;

		ChunkMesher chunk_mesher;
		const BlockMeshingContext context;
		
		void streamTerrain(StreamTarget target);
		Chunk* genChunk(Coordinates coordinates);
		float squaredDistance(glm::vec3 a, glm::vec3 b) const;
		Block blockAtWorldPos(WorldCoordinates coordinates);

		// queued chunk state
		std::priority_queue<ChunkGenRequest, std::vector<ChunkGenRequest>, std::greater<ChunkGenRequest>> queued_chunks;
		std::unordered_set<Coordinates, CoordinatesHash> queued_chunks_set;
		
		// generated chunks
		// TODO: wherever we use Chunk* use unique_ptr, for mem leaks and whatnot
		// Note: this should contain all *loaded* chunks, not all previously *generated* chunks. Some may have been loaded previously, and should not be generated again.
		std::unordered_map<Coordinates, Chunk*, CoordinatesHash> loaded_chunks;
		
		


	public:
		World(BlockMeshingContext context);
		std::vector<Chunk*> getVisibleChunks(StreamTarget target);
		void update(StreamTarget target);
};

#endif