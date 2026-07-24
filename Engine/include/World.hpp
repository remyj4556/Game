#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "Coordinates.hpp"
#include "FastNoiseLite.h"
#include "BlockDefinition.hpp"

#include "RenderRequest.hpp"
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <memory>
#include <utility>

struct WorldDebugInfo {
	int total_vertices_generated = 0;
	int num_generated = 0;
	int num_meshed = 0;
	std::chrono::duration<double, std::milli> total_gen_time;
	std::chrono::duration<double, std::milli> total_mesh_time;
	ChunkState current_chunk_state = ChunkState::Unloaded;
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

		// Contains chunks queued for loading (either from disk or new generation).
		// state is strictly ChunkState::QueuedToLoad
		std::priority_queue<ChunkLoadRequest, std::vector<ChunkLoadRequest>, std::greater<ChunkLoadRequest>> load_queue;
		std::unordered_set<CoordinateSystem::ChunkCoordinates, CoordinateSystem::ChunkCoordinatesHash> load_set;

		// Sole owner of actual Chunk objects (unique_ptr's). Everything else uses ChunkCoordinates to access from here.
		// state may be anything other than ChunkState::Unloaded
		std::unordered_map<CoordinateSystem::ChunkCoordinates, std::unique_ptr<Chunk>, CoordinateSystem::ChunkCoordinatesHash> loaded_chunks;

		// Contains chunks that need to be remeshed.
		// state is strictly ChunkState::QueuedToMesh
		std::queue<CoordinateSystem::ChunkCoordinates> dirty_chunks;

		// Mesh upload queue reference, owned by Game.
		// state is strictly ChunkState::Meshed
		std::queue<RenderRequest>& upload_queue;
		
		// Mesh unload queue reference, owned by Game.
		// state is strictly ChunkState::Uploaded
		std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue;
		
		void streamChunkLoads(StreamTarget target);
		void streamChunkUnloads(StreamTarget target);
		void loadChunks();
		void uploadChunkMeshes();
		void streamChunkMeshing();
		void genChunk(CoordinateSystem::ChunkCoordinates coordinates);
		float squaredDistance(glm::vec3 a, glm::vec3 b) const;
		block_id_type blockAtWorldPos(CoordinateSystem::WorldCoordinates world_coords);
		ChunkGroup getSurroundingChunks(CoordinateSystem::ChunkCoordinates chunk_coord) const;
		std::pair<bool, std::vector<CoordinateSystem::ChunkCoordinates>> getSurroundingChunkCoordinates(CoordinateSystem::ChunkCoordinates chunk_coord) const;
		void pruneLoadQueue(StreamTarget target);

	public:
		World(std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue);
		void update(StreamTarget target);
		CoordinateSystem::WorldCoordinates chunkToWorld(CoordinateSystem::ChunkCoordinates chunk_coords);
		CoordinateSystem::ChunkCoordinates worldToChunk(CoordinateSystem::WorldCoordinates world_coords);
		CoordinateSystem::WorldCoordinates localToWorld(CoordinateSystem::ChunkCoordinates chunk_coords, CoordinateSystem::LocalCoordinates local_coords);
		void unloadChunk(CoordinateSystem::ChunkCoordinates chunk_coord); // TODO: *probably* should not be public, only needs to be accessed by Game.
		void renderWorldDebugInfo();
};



#endif