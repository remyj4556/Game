#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/Coordinates.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/Vertex.hpp"
#include "../include/Mesh.hpp"
#include "../include/RenderRequest.hpp"

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

World::World(std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue)
	: last_streamed_chunk_coord({ 0, 0, 0 })
	, last_radius(0)
	, upload_queue(load_queue)
	, unload_queue(unload_queue)
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

	if (!loaded_chunks.contains(chunk_coords)) {
		block_id_type air = { 0 };
		return air;
	}
	
	return loaded_chunks.at(chunk_coords)->getBlock({ local_x, local_y, local_z });
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

	if (loaded_chunks.contains(current_chunk_coord))
		world_debug_info.current_chunk_state = loaded_chunks[current_chunk_coord]->state;

	// rebuild priority queue to ensure nearest/newest chunks are processed first
	//pruneLoadQueue(target);

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

				auto it = loaded_chunks.find(chunk_coord);
				Chunk* chunk_ptr = nullptr;

				// create ptr to new chunk if one does not already exist
				if (it == loaded_chunks.end()) {
					loaded_chunks[chunk_coord] = std::make_unique<Chunk>();
				}
				
				chunk_ptr = loaded_chunks[chunk_coord].get();

				// if chunk already exists and is past Unloaded, don't add it
				if (chunk_ptr->state != ChunkState::Unloaded) {
					continue;
				}
				
				// if chunk is unloaded and not already queued, advance state and push to load priority queue (and corresponding set)
				if (chunk_ptr->state == ChunkState::Unloaded && !load_set.contains(chunk_coord)) {
					chunk_ptr->advanceChunkState();

					if (chunk_ptr->state != ChunkState::QueuedToLoad) {
						std::cerr << "Error: " << errorToString(ErrorCodes::IncorrectChunkState) << "\n";
						exit(ErrorCodes::IncorrectChunkState);
					}

					load_queue.push({ chunk_coord, squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos) });
					load_set.insert(chunk_coord);
				}
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

	// iterate over loaded chunks, checking whether they are in render distance. If not, push to unload queue.
	for (const auto& [chunk_coord, chunk_ptr] : loaded_chunks) {
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

void World::genChunk(CoordinateSystem::ChunkCoordinates coordinates) {
	auto gen_start = std::chrono::high_resolution_clock::now();

	auto it = loaded_chunks.find(coordinates);
	Chunk* chunk_ptr = nullptr;

	if (it == loaded_chunks.end()) {
		std::cerr << "Generation Error: " << errorToString(ErrorCodes::ChunkDoesNotExist) << "\n";
		return;
	}

	chunk_ptr = it->second.get();

	if (chunk_ptr->state != ChunkState::QueuedToLoad) {
		return;
	}

	int chunk_size = Chunk::CHUNK_SIZE;

	for (int x = 0; x < chunk_size; ++x) {
		for (int z = 0; z < chunk_size; z++) {
			int height = pow(2, noise.GetNoise(static_cast<float>(coordinates.x * chunk_size + x), static_cast<float>(coordinates.z * chunk_size + z)) * 7);

			for (int y = 0; y < chunk_size; y++) {
				if ((coordinates.y * chunk_size + y) < height) {
					chunk_ptr->setBlock(CoordinateSystem::LocalCoordinates(x, y, z), 1);
				}
				if ((coordinates.y * chunk_size + y) == height) {
					chunk_ptr->setBlock(CoordinateSystem::LocalCoordinates(x, y, z), 3);
				}
			}
		}
	}

	// chunk is now loaded
	chunk_ptr->advanceChunkState();

	auto gen_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double, std::milli>(gen_end - gen_start);
	world_debug_info.total_gen_time += duration;
	world_debug_info.num_generated++;
}

void World::unloadChunk(CoordinateSystem::ChunkCoordinates chunk_coord) {
	// TODO: write to disk first if needed

	auto it = loaded_chunks.find(chunk_coord);
	Chunk* chunk_ptr = nullptr;

	if (it == loaded_chunks.end()) {
		std::cerr << "Unload Error: " << errorToString(ErrorCodes::ChunkDoesNotExist) << "\n";
		exit(ErrorCodes::ChunkDoesNotExist);
	}

	chunk_ptr = it->second.get();

	if (chunk_ptr->state != ChunkState::QueuedToUnload) {
		return;
	}

	loaded_chunks.erase(chunk_coord);
}

void World::loadChunks() {
	auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5);

	while (!load_queue.empty() && std::chrono::high_resolution_clock::now() < deadline) {
		ChunkLoadRequest current_load_request = load_queue.top();
		load_queue.pop();
		load_set.erase(current_load_request.coordinates);

		auto it = loaded_chunks.find(current_load_request.coordinates);
		Chunk* chunk_ptr = nullptr;

		if (it == loaded_chunks.end()) {
			continue;
		}

		chunk_ptr = it->second.get();

		if (chunk_ptr->state != ChunkState::QueuedToLoad) {
			continue;
		}

		// TODO: look for chunk on disk, skip generation if found

		// generate new chunk
		genChunk(current_load_request.coordinates);
	}
}

void World::streamChunkMeshing() {
	for (const auto& [chunk_coord, chunk_ptr] : loaded_chunks) {
		if (chunk_ptr->state != ChunkState::Loaded) {
			continue;
		}

		// ensure all neighboring chunks exist and are (at least) Loaded
		auto [complete_set, neighboring_coords] = getSurroundingChunkCoordinates(chunk_coord);
		if (!complete_set) {
			continue;
		}

		bool all_neighbors_ready = true;
		for (CoordinateSystem::ChunkCoordinates neighbor_coord : neighboring_coords) {
			auto it = loaded_chunks.find(neighbor_coord);

			if (it == loaded_chunks.end() ||
				it->second->state == ChunkState::Unloaded ||
				it->second->state == ChunkState::QueuedToLoad) {

				all_neighbors_ready = false;
				break;
			}
		}

		if (!all_neighbors_ready) {
			continue;
		}

		// enqueue chunk to be meshed
		chunk_ptr->advanceChunkState();

		if (chunk_ptr->state != ChunkState::QueuedToMesh) {
			std::cerr << "Error: " << errorToString(ErrorCodes::IncorrectChunkState) << "\n";
			exit(ErrorCodes::IncorrectChunkState);
		}

		dirty_chunks.push(chunk_coord);

		// enqueue neighbors to be (re)meshed
		for (CoordinateSystem::ChunkCoordinates neighbor_coord : neighboring_coords) {
			auto it = loaded_chunks.find(neighbor_coord);
			Chunk* neighbor_ptr = nullptr;

			if (it == loaded_chunks.end()) {
				continue;
			}

			neighbor_ptr = it->second.get();

			if (neighbor_ptr->state == ChunkState::Loaded ||
				neighbor_ptr->state == ChunkState::Meshed) {

				neighbor_ptr->state = ChunkState::QueuedToMesh;
				dirty_chunks.push(neighbor_coord);
			}
			
		}
	}
}

void World::uploadChunkMeshes() {
	auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(20);

	while (!dirty_chunks.empty() && std::chrono::high_resolution_clock::now() < deadline) {
		CoordinateSystem::ChunkCoordinates current_chunk_coords = dirty_chunks.front();
		dirty_chunks.pop();

		auto it = loaded_chunks.find(current_chunk_coords);
		Chunk* chunk_ptr = nullptr;

		if (it == loaded_chunks.end()) {
			continue;
		}

		chunk_ptr = it->second.get();

		if (chunk_ptr->state != ChunkState::QueuedToMesh) {
			continue;
		}

		auto mesh_start_time = std::chrono::high_resolution_clock::now();

		auto [mesh_vertices, mesh_indices] = chunk_mesher.buildGreedyMesh(getSurroundingChunks(current_chunk_coords));
		loaded_chunks[current_chunk_coords]->advanceChunkState();

		if (loaded_chunks[current_chunk_coords]->state != ChunkState::Meshed) {
			std::cerr << "Error: " << errorToString(ErrorCodes::IncorrectChunkState) << "\n";
			exit(ErrorCodes::IncorrectChunkState);
		}
		
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
	loadChunks();
	streamChunkMeshing();
	uploadChunkMeshes();
}

ChunkGroup World::getSurroundingChunks(CoordinateSystem::ChunkCoordinates chunk_coord) const {
	ChunkGroup group;

	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z })) {
		group.main = loaded_chunks.at({ chunk_coord.x, chunk_coord.y, chunk_coord.z }).get();
	}
 	if (loaded_chunks.contains({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z })) {
		group.left = loaded_chunks.at({ chunk_coord.x - 1, chunk_coord.y, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z })) {
		group.right = loaded_chunks.at({ chunk_coord.x + 1, chunk_coord.y, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z })) {
		group.bottom = loaded_chunks.at({ chunk_coord.x, chunk_coord.y - 1, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z })) {
		group.top = loaded_chunks.at({ chunk_coord.x, chunk_coord.y + 1, chunk_coord.z }).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1 })) {
		group.back = loaded_chunks.at({ chunk_coord.x, chunk_coord.y, chunk_coord.z - 1}).get();
	}
	if (loaded_chunks.contains({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 })) {
		group.front = loaded_chunks.at({ chunk_coord.x, chunk_coord.y, chunk_coord.z + 1 }).get();
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

void World::pruneLoadQueue(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;
	load_queue = std::priority_queue<ChunkLoadRequest, std::vector<ChunkLoadRequest>, std::greater<ChunkLoadRequest>>();
	std::vector<CoordinateSystem::ChunkCoordinates> to_remove;

	for (CoordinateSystem::ChunkCoordinates chunk_coord : load_set) {
		float distance = squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos);

		if (distance > (target.chunk_load_radius * target.chunk_load_radius)) {
			to_remove.push_back(chunk_coord);
			continue;
		}

		load_queue.push({ chunk_coord, distance });
	}

	for (CoordinateSystem::ChunkCoordinates chunk_coord : to_remove) {
		load_set.erase(chunk_coord);
	}
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
	ImGui::Text("Total Chunks Loaded: %i", loaded_chunks.size());
	ImGui::Text("Chunk Loading Queue: %i", load_set.size());
	ImGui::Text("Chunk Meshing Queue: %i", dirty_chunks.size());
	ImGui::Text("Current Chunk State: %s", Chunk::chunkStateToString(world_debug_info.current_chunk_state).c_str());
}

