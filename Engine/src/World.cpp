#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/Coordinates.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/Mesh.hpp"
#include "../include/RenderRequest.hpp"
#include "../include/ThreadPool.hpp"
#include "../include/ThreadSafeQueue.hpp"

#include <vector>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <utility>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <iostream>
#include <queue>
#include <ratio>
#include <string>
#include <cstdlib>

enum ErrorCodes : int {
	IncorrectChunkState,
	ChunkDoesNotExist
};

static std::string errorToString(ErrorCodes error_code) {
	switch (error_code) {
		case ErrorCodes::IncorrectChunkState:
			return "Incorrect Chunk State";
		case ErrorCodes::ChunkDoesNotExist:
			return "Chunk Does Not Exist";
	}
}

World::World(std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue, ThreadPool &thread_pool)
	: last_streamed_chunk_coord({ 0, 0, 0 })
	, last_radius(0)
	, upload_queue(load_queue)
	, unload_queue(unload_queue)
	, thread_pool(thread_pool)
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.02);
}

CoordinateSystem::WorldCoordinates World::chunkToWorld(CoordinateSystem::ChunkCoordinates chunk_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	CoordinateSystem::WorldCoordinates c;
	c.x = chunk_coords.x * chunk_size;
	c.y = chunk_coords.y * chunk_size;
	c.z = chunk_coords.z * chunk_size;

	return c;
}

CoordinateSystem::ChunkCoordinates World::worldToChunk(CoordinateSystem::WorldCoordinates world_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	CoordinateSystem::ChunkCoordinates c;
	c.x = std::floor(static_cast<double>(world_coords.x) / chunk_size);
	c.y = std::floor(static_cast<double>(world_coords.y) / chunk_size);
	c.z = std::floor(static_cast<double>(world_coords.z) / chunk_size);

	return c;
}

CoordinateSystem::WorldCoordinates World::localToWorld(CoordinateSystem::ChunkCoordinates chunk_coords, CoordinateSystem::LocalCoordinates local_coords) {
	CoordinateSystem::WorldCoordinates world_coords = chunkToWorld(chunk_coords);

	world_coords.x += local_coords.x;
	world_coords.y += local_coords.y;
	world_coords.z += local_coords.z;

	return world_coords;
}

block_id_type World::blockAtWorldPos(CoordinateSystem::WorldCoordinates world_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	CoordinateSystem::ChunkCoordinates chunk_coords = worldToChunk(world_coords);
	uint8_t local_x = world_coords.x - chunk_coords.x * chunk_size;
	uint8_t local_y = world_coords.y - chunk_coords.y * chunk_size;
	uint8_t local_z = world_coords.z - chunk_coords.z * chunk_size;

	auto chunk_ptr = loaded_chunks.tryGet(chunk_coords);

	if (!chunk_ptr || chunk_ptr->state == ChunkState::Unloaded || chunk_ptr->state == ChunkState::QueuedToLoad) {
		block_id_type air = { 0 };
		return air;
	}

	return chunk_ptr->getBlockFast(local_x, local_y, local_z);
}

void World::streamChunkLoads(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;

	// get target position
	CoordinateSystem::ChunkCoordinates current_chunk_coord;
	current_chunk_coord.x = std::floor(static_cast<double>(target.pos.x) / chunk_size);
	current_chunk_coord.y = std::floor(static_cast<double>(target.pos.y) / chunk_size);
	current_chunk_coord.z = std::floor(static_cast<double>(target.pos.z) / chunk_size);

	if (current_chunk_coord == last_streamed_chunk_coord && target.chunk_load_radius == last_radius) {
		return;
	}

	auto current_chunk_ptr = loaded_chunks.tryGet(current_chunk_coord);
	if (current_chunk_ptr)
		world_debug_info.current_chunk_state = current_chunk_ptr->state;

	last_streamed_chunk_coord = current_chunk_coord;
	last_radius = target.chunk_load_radius;

	// access all neighboring chunks inside of chunk render distance
	for (int x = -target.chunk_load_radius; x <= target.chunk_load_radius; ++x) {
		for (int z = -target.chunk_load_radius; z <= target.chunk_load_radius; ++z) {
			for (int y = -target.chunk_load_radius; y <= target.chunk_load_radius; ++y) {
				CoordinateSystem::ChunkCoordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to make a sphere
				if ((x * x + y * y + z * z) > (target.chunk_load_radius * target.chunk_load_radius)) {
					continue;
				}

				auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);
				
				// create new chunk if one does not exist
				if (!chunk_ptr) {
					chunk_ptr = std::make_shared<Chunk>();
					loaded_chunks.insert({ chunk_coord, chunk_ptr });
				}

				// if chunk state is past Unloaded, don't submit load task
				if (chunk_ptr->state != ChunkState::Unloaded) {
					continue;
				}
				
				// otherwise chunk is Unloaded, advance state to QueuedToLoad and submit load task to thread pool
				chunk_ptr->state = ChunkState::QueuedToLoad;
				thread_pool.enqueueTask(&World::loadChunk, this, chunk_coord);
			}
		}
	}
}

void World::streamChunkUnloads(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;

	// get target position
	CoordinateSystem::ChunkCoordinates current_chunk_coord;
	current_chunk_coord.x = std::floor(static_cast<double>(target.pos.x) / chunk_size);
	current_chunk_coord.y = std::floor(static_cast<double>(target.pos.y) / chunk_size);
	current_chunk_coord.z = std::floor(static_cast<double>(target.pos.z) / chunk_size);

	// get all existing loaded chunk coordinates
	std::vector<CoordinateSystem::ChunkCoordinates> loaded_chunk_coords = loaded_chunks.getAllCoords();

	// iterate over loaded chunks, checking whether they are in render distance. If not, push to unload queue.
	for (auto chunk_coord : loaded_chunk_coords) {
		auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);

		if (!chunk_ptr) {
			continue;
		}

		if (chunk_ptr->state == ChunkState::Unloaded ||
			chunk_ptr->state == ChunkState::QueuedToUnload) {
			continue;
		}

		glm::vec3 chunk_position = glm::vec3(chunk_coord.x, chunk_coord.y, chunk_coord.z);
		glm::vec3 target_chunk_position = glm::vec3(current_chunk_coord.x, current_chunk_coord.y, current_chunk_coord.z);
		float distance = squaredDistance(chunk_position, target_chunk_position);

		if (distance > (target.chunk_load_radius * target.chunk_load_radius)) {
			chunk_ptr->state = ChunkState::QueuedToUnload;
			unload_queue.push(chunk_coord);
		}
	}
}

void World::genChunk(CoordinateSystem::ChunkCoordinates chunk_coord) {
	auto gen_start = std::chrono::high_resolution_clock::now();
	int chunk_size = Chunk::CHUNK_SIZE;

	auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);

	if (!chunk_ptr || (chunk_ptr->state != ChunkState::QueuedToLoad)) {
		return;
	}

	for (int x = 0; x < chunk_size; ++x) {
		for (int z = 0; z < chunk_size; z++) {
			int height = pow(2, noise.GetNoise(static_cast<float>(chunk_coord.x * chunk_size + x), static_cast<float>(chunk_coord.z * chunk_size + z)) * 7);

			for (int y = 0; y < chunk_size; y++) {
				if ((chunk_coord.y * chunk_size + y) < height) {
					chunk_ptr->setBlock(CoordinateSystem::LocalCoordinates(x, y, z), 1);
				}
				if ((chunk_coord.y * chunk_size + y) == height) {
					chunk_ptr->setBlock(CoordinateSystem::LocalCoordinates(x, y, z), 3);
				}
			}
		}
	}

	chunk_ptr->state = ChunkState::Loaded;

	auto gen_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double, std::milli>(gen_end - gen_start);
	world_debug_info.total_gen_time += duration;
	world_debug_info.num_generated++;
}

void World::unloadChunk(CoordinateSystem::ChunkCoordinates chunk_coord) {
	// TODO: write to disk first if needed

	auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);

	if (!chunk_ptr || chunk_ptr->state != ChunkState::QueuedToUnload) {
		return;
	}

	loaded_chunks.erase(chunk_coord);
}

void World::loadChunk(CoordinateSystem::ChunkCoordinates chunk_coord) {
	auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);

	if (!chunk_ptr || chunk_ptr->state != ChunkState::QueuedToLoad) {
		return;
	}

	// TODO: look for chunk on disk, skip generation if found

	// generate new chunk
	genChunk(chunk_coord);
}

void World::streamChunkMeshing(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;

	// get target position
	CoordinateSystem::ChunkCoordinates current_chunk_coord;
	current_chunk_coord.x = std::floor(static_cast<double>(target.pos.x) / chunk_size);
	current_chunk_coord.y = std::floor(static_cast<double>(target.pos.y) / chunk_size);
	current_chunk_coord.z = std::floor(static_cast<double>(target.pos.z) / chunk_size);

	for (int x = -target.chunk_load_radius; x <= target.chunk_load_radius; ++x) {
		for (int z = -target.chunk_load_radius; z <= target.chunk_load_radius; ++z) {
			for (int y = -target.chunk_load_radius; y <= target.chunk_load_radius; ++y) {
				CoordinateSystem::ChunkCoordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to make a sphere
				if ((x * x + y * y + z * z) > (target.chunk_load_radius * target.chunk_load_radius)) {
					continue;
				}

				auto chunk_ptr = loaded_chunks.tryGet(chunk_coord);

				if (!chunk_ptr || chunk_ptr->state != ChunkState::Loaded) {
					continue;
				}

				auto [complete_set, neighboring_coords] = getSurroundingChunkCoordinates(chunk_coord);
				if (!complete_set) {
					continue;
				}

				bool all_neighbors_ready = true;
				for (CoordinateSystem::ChunkCoordinates neighbor_coord : neighboring_coords) {
					auto neighbor_ptr = loaded_chunks.tryGet(neighbor_coord);

					if (!neighbor_ptr ||
						neighbor_ptr->state == ChunkState::Unloaded ||
						neighbor_ptr->state == ChunkState::QueuedToLoad) {

						all_neighbors_ready = false;
						break;
					}
				}

				if (!all_neighbors_ready) {
					continue;
				}

				// chunk is ready to be remeshed
				chunk_ptr->state = ChunkState::QueuedToMesh;

				// TODO: may not need this, just submit meshing jobs directly to thread pool
				dirty_chunks.push(chunk_coord);

				// enqueue neighbors to be (re)meshed
				for (CoordinateSystem::ChunkCoordinates neighbor_coord : neighboring_coords) {
					auto neighbor_ptr = loaded_chunks.tryGet(neighbor_coord);

					if (!neighbor_ptr) {
						continue;
					}

					if (neighbor_ptr->state == ChunkState::Loaded ||
						neighbor_ptr->state == ChunkState::Meshed) {

						neighbor_ptr->state = ChunkState::QueuedToMesh;
						dirty_chunks.push(neighbor_coord);
					}
				}
			}
		}
	}
}

void World::uploadChunkMeshes() {
	auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(20);

	while (!dirty_chunks.empty() && std::chrono::high_resolution_clock::now() < deadline) {
		CoordinateSystem::ChunkCoordinates current_chunk_coords = dirty_chunks.front();
		dirty_chunks.pop();

		auto chunk_ptr = loaded_chunks.tryGet(current_chunk_coords);

		if (!chunk_ptr || chunk_ptr->state != ChunkState::QueuedToMesh) {
			continue;
		}

		auto mesh_start_time = std::chrono::high_resolution_clock::now();

		auto [mesh_vertices, mesh_indices] = chunk_mesher.buildGreedyMesh(getSurroundingChunks(current_chunk_coords));
		chunk_ptr->state = ChunkState::Meshed;

		upload_queue.push(RenderRequest(current_chunk_coords, Mesh(mesh_vertices, mesh_indices)));

		auto mesh_end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<double, std::milli>(mesh_end_time - mesh_start_time);
		world_debug_info.total_mesh_time += duration;
		world_debug_info.num_meshed++;
		world_debug_info.total_vertices_generated += mesh_vertices.size();
	}
}

void World::update(StreamTarget target) {
	streamChunkLoads(target);
	streamChunkUnloads(target);
	streamChunkMeshing(target);
	uploadChunkMeshes();
}

ChunkGroup World::getSurroundingChunks(CoordinateSystem::ChunkCoordinates chunk_coord) {
	ChunkGroup group;

	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z })) {
		group.main = loaded_chunks.tryGet({ chunk_coord.x, chunk_coord.y, chunk_coord.z }).get();
	}
 	if (loaded_chunks.contains({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z })) {
		group.left = loaded_chunks.tryGet({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z })) {
		group.right = loaded_chunks.tryGet({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z })) {
		group.bottom = loaded_chunks.tryGet({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z })) {
		group.top = loaded_chunks.tryGet({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1 })) {
		group.back = loaded_chunks.tryGet({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1}).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 })) {
		group.front = loaded_chunks.tryGet({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 }).get();
	}

	return group;
}

std::pair<bool, std::vector<CoordinateSystem::ChunkCoordinates>> World::getSurroundingChunkCoordinates(CoordinateSystem::ChunkCoordinates chunk_coord) const {
	std::vector<CoordinateSystem::ChunkCoordinates> neighbors;

	if (loaded_chunks.contains({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z })) {
		neighbors.push_back({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z });
	}
	if (loaded_chunks.contains({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z })) {
		neighbors.push_back({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z });
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z })) {
		neighbors.push_back({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z });
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z })) {
		neighbors.push_back({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z });
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1 })) {
		neighbors.push_back({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1});
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 })) {
		neighbors.push_back({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 });
	}

	bool complete_set = neighbors.size() == 6;

	return { complete_set, neighbors };
}

float World::squaredDistance(glm::vec3 a, glm::vec3 b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return (dx * dx) + (dy * dy) + (dz * dz);
}

void World::renderWorldDebugInfo() {
	ImGui::Text("Avg Mesh Time: %fms", world_debug_info.total_mesh_time / world_debug_info.num_meshed);
	ImGui::Text("Avg Gen Time: %fms", world_debug_info.total_gen_time / world_debug_info.num_generated);
	ImGui::Text("Avg Chunk Vertex Count: %f", (float)world_debug_info.total_vertices_generated / world_debug_info.num_meshed);
	//ImGui::Text("Total Chunks Loaded: %i", loaded_chunks.size());
	ImGui::Text("Chunk Meshing Queue: %i", dirty_chunks.size());
	ImGui::Text("Current Chunk State: %s", Chunk::chunkStateToString(world_debug_info.current_chunk_state).c_str());
}

