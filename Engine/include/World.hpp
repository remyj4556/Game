#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "Coordinates.hpp"
#include "FastNoiseLite.h"
#include "BlockDefinition.hpp"
#include "ThreadPool.hpp"
#include "ThreadSafeQueue.hpp"
#include "RenderRequest.hpp"
#include "ChunkMap.hpp"

#include <vector>
#include <chrono>
#include <memory>
#include <utility>
#include <ratio>
#include <atomic>
#include <mutex>

struct WorldDebugInfo {
	std::atomic<ChunkState> current_chunk_state = ChunkState::Unloaded;
	std::atomic<int> total_vertices_generated = 0;
	std::atomic<int> num_generated = 0;
	std::atomic<int> num_meshed = 0;

	// += for double not supported under atomic, just lock mutex instead when modifying
	std::chrono::duration<double, std::milli> total_gen_time;
	std::chrono::duration<double, std::milli> total_mesh_time;

	std::mutex debug_info_mutex;
};

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

		FastNoiseLite noise;
		ChunkMesher chunk_mesher;
		WorldDebugInfo world_debug_info;
		ThreadPool& thread_pool;

		// Sole owner of actual Chunk objects (shared_ptr's for multithreading ease). Everything else uses ChunkCoordinates to access from here.
		// state may be anything other than ChunkState::Unloaded
		ChunkMap loaded_chunks;

		// Mesh upload queue reference, owned by Game.
		// state is strictly ChunkState::Meshed
		ThreadSafeQueue<RenderRequest>& upload_queue;
		
		// Mesh unload queue reference, owned by Game.
		// state is strictly ChunkState::Uploaded
		ThreadSafeQueue<CoordinateSystem::ChunkCoordinates>& unload_queue;
		
		void streamChunkLoads(StreamTarget target);
		void streamChunkUnloads(StreamTarget target);
		void loadChunk(CoordinateSystem::ChunkCoordinates chunk_coord);
		void meshChunk(CoordinateSystem::ChunkCoordinates chunk_coord);
		void streamChunkMeshing(StreamTarget target);
		void genChunk(CoordinateSystem::ChunkCoordinates coordinates);
		float squaredDistance(glm::vec3 a, glm::vec3 b) const;
		block_id_type blockAtWorldPos(CoordinateSystem::WorldCoordinates world_coords);
		ChunkGroup getSurroundingChunks(CoordinateSystem::ChunkCoordinates chunk_coord);
		std::pair<bool, std::vector<CoordinateSystem::ChunkCoordinates>> getSurroundingChunkCoordinates(CoordinateSystem::ChunkCoordinates chunk_coord) const;

	public:
		World(ThreadSafeQueue<RenderRequest>& load_queue, ThreadSafeQueue<CoordinateSystem::ChunkCoordinates>& unload_queue, ThreadPool& thread_pool);
		void update(StreamTarget target);
		CoordinateSystem::WorldCoordinates chunkToWorld(CoordinateSystem::ChunkCoordinates chunk_coords);
		CoordinateSystem::ChunkCoordinates worldToChunk(CoordinateSystem::WorldCoordinates world_coords);
		CoordinateSystem::WorldCoordinates localToWorld(CoordinateSystem::ChunkCoordinates chunk_coords, CoordinateSystem::LocalCoordinates local_coords);
		void unloadChunk(CoordinateSystem::ChunkCoordinates chunk_coord);
		void renderWorldDebugInfo();
};



#endif