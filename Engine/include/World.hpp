#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "Coordinates.hpp"
#include "Coordinates.hpp"
#include "Coordinates.hpp"
#include "FastNoiseLite.h"
#include "Mesh.hpp"
#include "Block.hpp"
#include "RenderRequest.hpp"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <memory>

struct StreamTarget {
	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	int chunk_load_radius = 0;
};

struct ChunkLoadRequest {
	CoordinateSystem::ChunkCoordinates coordinates;
	float dist_to_target;

	bool operator>(const ChunkLoadRequest& other) const {
		return dist_to_target > other.dist_to_target;
	}
};

class World {
	private:
		CoordinateSystem::ChunkCoordinates last_streamed_chunk_coord;
		int last_radius;

		// references to queues owned by Game
		std::queue<RenderRequest>& load_queue;
		std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue;

		FastNoiseLite noise;
		ChunkMesher chunk_mesher;
		std::priority_queue<ChunkLoadRequest, std::vector<ChunkLoadRequest>, std::greater<ChunkLoadRequest>> queued_chunks_to_load;
		std::unordered_set<CoordinateSystem::ChunkCoordinates, CoordinateSystem::ChunkCoordinatesHash> unique_queued_chunks_to_load;
		std::unordered_map<CoordinateSystem::ChunkCoordinates, std::unique_ptr<Chunk>, CoordinateSystem::ChunkCoordinatesHash> loaded_chunks;
		std::queue<CoordinateSystem::ChunkCoordinates> dirty_chunks;
		
		// TODO: push data to a DebugRegistry or something, which ImGUI can then read from independently.
		int total_vertices_generated = 0;
		int num_generated = 0;
		int num_meshed = 0;
		std::chrono::duration<double, std::milli> total_gen_time;
		std::chrono::duration<double, std::milli> total_mesh_time;

		void streamTerrain(StreamTarget target);
		void loadQueuedChunks();
		void enqueueChunkMeshes();
		std::unique_ptr<Chunk> genChunk(CoordinateSystem::ChunkCoordinates coordinates);
		float squaredDistance(glm::vec3 a, glm::vec3 b) const;
		Block blockAtWorldPos(CoordinateSystem::WorldCoordinates world_coords);
		ChunkGroup getSurroundingChunks(CoordinateSystem::ChunkCoordinates chunk_coord) const;
		std::vector<CoordinateSystem::ChunkCoordinates> getSurroundingChunkCoordinates(CoordinateSystem::ChunkCoordinates chunk_coord) const;

	public:
		World(std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue);
		void update(StreamTarget target);
		CoordinateSystem::WorldCoordinates chunkToWorld(CoordinateSystem::ChunkCoordinates chunk_coords);
		CoordinateSystem::ChunkCoordinates worldToChunk(CoordinateSystem::WorldCoordinates world_coords);
		CoordinateSystem::WorldCoordinates localToWorld(CoordinateSystem::ChunkCoordinates chunk_coords, CoordinateSystem::LocalCoordinates local_coords);
		void unloadChunk(CoordinateSystem::ChunkCoordinates chunk_coord);
};

#endif