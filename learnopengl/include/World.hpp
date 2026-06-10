#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "ChunkCoordinates.hpp"
#include "WorldCoordinates.hpp"
#include "LocalCoordinates.hpp"
#include "BlockMeshingContext.hpp"
#include "FastNoiseLite.h"
#include "Block.hpp"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <chrono>

struct StreamTarget {
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	int chunk_load_radius = 0;
};

struct ChunkGenRequest {
	ChunkCoordinates coordinates;
	float dist_to_target;

	bool operator>(const ChunkGenRequest& other) const {
		return dist_to_target > other.dist_to_target;
	}
};

class World {
	private:
		ChunkCoordinates last_streamed_chunk_coord;
		int last_radius;

		FastNoiseLite noise;

		ChunkMesher chunk_mesher;
		const BlockMeshingContext context;
		
		void streamTerrain(StreamTarget target);
		Chunk* genChunk(ChunkCoordinates coordinates);
		float squaredDistance(glm::vec3 a, glm::vec3 b) const;
		Block blockAtWorldPos(WorldCoordinates world_coords);
		ChunkGroup getSurroundingChunks(Chunk* chunk);

		// queued chunk state
		std::priority_queue<ChunkGenRequest, std::vector<ChunkGenRequest>, std::greater<ChunkGenRequest>> queued_chunks;
		std::unordered_set<ChunkCoordinates, CoordinatesHash> queued_chunks_set;
		
		// generated chunks
		// TODO: wherever we use Chunk* use unique_ptr, for mem leaks and whatnot
		// Note: this should contain all *loaded* chunks, not all previously *generated* chunks. Some may have been loaded previously, and should not be generated again.
		std::unordered_map<ChunkCoordinates, Chunk*, CoordinatesHash> loaded_chunks;
		
		// for profiling
		int total_vertices_generated = 0;
		int num_generated = 0;
		int num_meshed = 0;
		std::chrono::duration<double, std::milli> total_gen_time;
		std::chrono::duration<double, std::milli> total_mesh_time;
		

	public:
		World(BlockMeshingContext context);
		std::vector<Chunk*> getVisibleChunks(StreamTarget target);
		void update(StreamTarget target);
		WorldCoordinates chunkToWorld(ChunkCoordinates chunk_coords);
		ChunkCoordinates worldToChunk(WorldCoordinates world_coords);
		WorldCoordinates localToWorld(ChunkCoordinates chunk_coords, LocalCoordinates local_coords);
};

#endif