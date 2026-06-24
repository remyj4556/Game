#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/WorldCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/Block.hpp"
#include "../include/Vertex.hpp"
#include "../include/Mesh.hpp"
#include "../include/ChunkRenderRequest.hpp"

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

World::World(std::queue<ChunkRenderRequest>& chunk_render_queue, std::queue<ChunkCoordinates>& chunk_unload_queue)
	: last_streamed_chunk_coord({ 0, 0, 0 })
	, last_radius(0)
	, chunk_render_queue(chunk_render_queue)
	, chunk_unload_queue(chunk_unload_queue)
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.02);
}

WorldCoordinates World::chunkToWorld(ChunkCoordinates chunk_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	WorldCoordinates c;
	c.x = chunk_coords.x * chunk_size;
	c.y = chunk_coords.y * chunk_size;
	c.z = chunk_coords.z * chunk_size;

	return c;
}

ChunkCoordinates World::worldToChunk(WorldCoordinates world_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	ChunkCoordinates c;
	c.x = std::floor(static_cast<double>(world_coords.x) / chunk_size);
	c.y = std::floor(static_cast<double>(world_coords.y) / chunk_size);
	c.z = std::floor(static_cast<double>(world_coords.z) / chunk_size);

	return c;
}

WorldCoordinates World::localToWorld(ChunkCoordinates chunk_coords, LocalCoordinates local_coords) {
	WorldCoordinates world_coords = chunkToWorld(chunk_coords);

	world_coords.x += local_coords.x;
	world_coords.y += local_coords.y;
	world_coords.z += local_coords.z;

	return world_coords;
}

Block World::blockAtWorldPos(WorldCoordinates world_coords) {
	int chunk_size = Chunk::CHUNK_SIZE;

	ChunkCoordinates chunk_coords = worldToChunk(world_coords);
	uint8_t local_x = world_coords.x - chunk_coords.x * chunk_size;
	uint8_t local_y = world_coords.y - chunk_coords.y * chunk_size;
	uint8_t local_z = world_coords.z - chunk_coords.z * chunk_size;

	if (!loaded_chunks.contains(chunk_coords)) {
		Block air = { 0 };
		return air;
	}
	
	return loaded_chunks.at(chunk_coords)->getBlock({ local_x, local_y, local_z });
}

void World::streamTerrain(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;

	// get target position
	ChunkCoordinates current_chunk_coord;
	current_chunk_coord.x = std::floor(static_cast<double>(target.pos.x) / chunk_size);
	current_chunk_coord.y = std::floor(static_cast<double>(target.pos.y) / chunk_size);
	current_chunk_coord.z = std::floor(static_cast<double>(target.pos.z) / chunk_size);

	if (current_chunk_coord == last_streamed_chunk_coord && target.chunk_load_radius == last_radius) {
		return;
	}

	// rebuild priority queue to ensure nearest/newest chunks are processed first
	queued_chunks_to_load = std::priority_queue<ChunkLoadRequest, std::vector<ChunkLoadRequest>, std::greater<ChunkLoadRequest>>();
	std::vector<ChunkCoordinates> to_remove;
	for (ChunkCoordinates chunk_coord : unique_queued_chunks_to_load) {
		float distance = squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos);

		if (distance > (target.chunk_load_radius * target.chunk_load_radius)) {
			to_remove.push_back(chunk_coord);
			continue;
		}

		queued_chunks_to_load.push({ chunk_coord, distance });
	}

	for (ChunkCoordinates chunk_coord : to_remove) {
		unique_queued_chunks_to_load.erase(chunk_coord);
	}

	last_streamed_chunk_coord = current_chunk_coord;
	last_radius = target.chunk_load_radius;

	for (int x = -target.chunk_load_radius; x <= target.chunk_load_radius; ++x) {
		for (int z = -target.chunk_load_radius; z <= target.chunk_load_radius; ++z) {
			for (int y = -target.chunk_load_radius; y <= target.chunk_load_radius; ++y) {
				// get coordinates of neighboring chunk
				ChunkCoordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to check a sphere
				if ((x * x + y * y + z * z) > (target.chunk_load_radius * target.chunk_load_radius)) {
					continue;
				}

				// skip if chunk is already loaded
				if (loaded_chunks.contains(chunk_coord)) {
					continue;
				}

				// skip if already in set of queued chunks to generate
				if (unique_queued_chunks_to_load.contains(chunk_coord)) {
					continue;
				}

				// add chunk to queue to generate
				queued_chunks_to_load.push({ chunk_coord, squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos) });
				unique_queued_chunks_to_load.insert(chunk_coord);
			}
		}
	}

	// iterate over loaded chunks, checking whether they are in render distance. If not, push to unload queue.
	for (const auto& [chunk_coord, chunk_ptr] : loaded_chunks) {
		glm::vec3 chunk_position = glm::vec3(chunk_coord.x, chunk_coord.y, chunk_coord.z);
		glm::vec3 target_chunk_position = glm::vec3(current_chunk_coord.x, current_chunk_coord.y, current_chunk_coord.z);
		float distance = squaredDistance(chunk_position, target_chunk_position);

		if (distance > (target.chunk_load_radius * target.chunk_load_radius)) {
			chunk_unload_queue.push(ChunkCoordinates(chunk_coord.x, chunk_coord.y, chunk_coord.z));
		}
	}
}

std::unique_ptr<Chunk> World::genChunk(ChunkCoordinates coordinates) {
	auto gen_start = std::chrono::high_resolution_clock::now();

	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(coordinates);
	int chunk_size = Chunk::CHUNK_SIZE;
	
	for (uint8_t x = 0; x < chunk_size; ++x) {
		for (uint8_t z = 0; z < chunk_size; z++) {
			int height = pow(2, noise.GetNoise(static_cast<float>(coordinates.x * chunk_size + x), static_cast<float>(coordinates.z * chunk_size + z)) * 7);

			for (uint8_t y = 0; y < chunk_size; y++) {
				if ((coordinates.y * chunk_size + y) < height) {
					chunk->setBlock(LocalCoordinates(x,y,z), Block(1));
				}
				if ((coordinates.y * chunk_size + y) == height) {
					chunk->setBlock(LocalCoordinates(x, y, z), Block(3));
				}
			}
		
		}
	}

	auto gen_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration<double, std::milli>(gen_end - gen_start);
	total_gen_time += duration;
	num_generated++;
	
	return chunk;
}

void World::unloadChunk(ChunkCoordinates chunk_coord) {
	// TODO: write to disk
	loaded_chunks.erase(chunk_coord);
}

void World::loadQueuedChunks() {
	auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5);

	while (!queued_chunks_to_load.empty() && std::chrono::high_resolution_clock::now() < deadline) {
		ChunkLoadRequest current_load_request = queued_chunks_to_load.top();
		queued_chunks_to_load.pop();

		// skip chunk if already loaded
		if (loaded_chunks.contains(current_load_request.coordinates)) {
			unique_queued_chunks_to_load.erase(current_load_request.coordinates);
			continue;
		}

		// TODO: look for chunk on disk, skip generation if found

		// generate new chunk
		loaded_chunks[current_load_request.coordinates] = genChunk(current_load_request.coordinates);
		unique_queued_chunks_to_load.erase(current_load_request.coordinates);

		// enqueue chunk and neighbors to dirty queue to be (re)meshed
		dirty_chunks.push(current_load_request.coordinates);
		for (ChunkCoordinates chunk_coord : getSurroundingChunkCoordinates(current_load_request.coordinates)) {
			dirty_chunks.push(chunk_coord);
		}
	}
}

void World::enqueueChunkMeshes() {
	auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(50);

	while (!dirty_chunks.empty() && std::chrono::high_resolution_clock::now() < deadline) {
		ChunkCoordinates current_chunk_coords = dirty_chunks.front();
		dirty_chunks.pop();

		auto mesh_start_time = std::chrono::high_resolution_clock::now();

		auto [mesh_vertices, mesh_indices] = chunk_mesher.buildGreedyMesh(getSurroundingChunks(current_chunk_coords));
		chunk_render_queue.push(ChunkRenderRequest(current_chunk_coords, Mesh(mesh_vertices, mesh_indices)));

		auto mesh_end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<double, std::milli>(mesh_end_time - mesh_start_time);
		total_mesh_time += duration;
		num_meshed++;
		total_vertices_generated += mesh_vertices.size();
	}
}

void World::update(StreamTarget target) {
	int chunk_size = Chunk::CHUNK_SIZE;

	// profiling ---------
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::Begin("Render Info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Avg Mesh Time: %fms", total_mesh_time / num_meshed);
	ImGui::Text("Avg Gen Time: %fms", total_gen_time / num_generated);
	ImGui::Text("Avg Chunk Vertex Count: %f", (float)total_vertices_generated / num_meshed);
	ImGui::End();
	// -------------------

	streamTerrain(target);
	loadQueuedChunks();
	enqueueChunkMeshes();
}

ChunkGroup World::getSurroundingChunks(ChunkCoordinates chunk_coord) const {
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

std::vector<ChunkCoordinates> World::getSurroundingChunkCoordinates(ChunkCoordinates chunk_coord) const {
	std::vector<ChunkCoordinates> neighbors;

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

	return neighbors;
}

float World::squaredDistance(glm::vec3 a, glm::vec3 b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return (dx * dx) + (dy * dy) + (dz * dz);
}

