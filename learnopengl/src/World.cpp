#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/WorldCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/Block.hpp"
#include "../include/Vertex.hpp"
#include "../include/Mesh.hpp"

#include <vector>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

World::World() : last_streamed_chunk_coord({ 0, 0, 0 }), last_radius(0) {
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

std::vector<Chunk*> World::getVisibleChunks(StreamTarget target) {
	std::vector<Chunk*> visible_chunks;

	int chunk_size = Chunk::CHUNK_SIZE;

	// this returns all chunks that are within the target's load radius
	for (auto& chunk : loaded_chunks) {
		int cx = std::floor(static_cast<double>(target.pos.x) / chunk_size);
		int cy = std::floor(static_cast<double>(target.pos.y) / chunk_size);
		int cz = std::floor(static_cast<double>(target.pos.z) / chunk_size);

		int dx = abs(chunk.second->getChunkPosition().x - cx);
		int dy = abs(chunk.second->getChunkPosition().y - cy);
		int dz = abs(chunk.second->getChunkPosition().z - cz);

		if (dx <= target.chunk_load_radius &&
			dy <= target.chunk_load_radius &&
			dz <= target.chunk_load_radius) {

			visible_chunks.push_back(chunk.second);
		}
	}

	return visible_chunks;
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

	last_streamed_chunk_coord = current_chunk_coord;
	last_radius = target.chunk_load_radius;

	for (int x = -target.chunk_load_radius; x <= target.chunk_load_radius; ++x) {
		for (int z = -target.chunk_load_radius; z <= target.chunk_load_radius; ++z) {
			for (int y = -target.chunk_load_radius; y <= target.chunk_load_radius; ++y) {
				// get coordinates of surrounding chunks
				ChunkCoordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to load a sphere
				if ((x * x + y * y + z * z) > (target.chunk_load_radius * target.chunk_load_radius)) {
					continue;
				}


				// skip if chunk is already loaded
				if (loaded_chunks.contains(chunk_coord)) {
					//std::cout << "chunk at: " << chunk_coord << " is already loaded\n";
					continue;
				}

				// skip if already in set of queued chunks to generate
				if (queued_chunks_set.contains(chunk_coord)) {
					continue;
				}


				// add chunk to queue to load/generate
				if (!loaded_chunks.contains(chunk_coord) && !queued_chunks_set.contains(chunk_coord)) {
					queued_chunks.push({ chunk_coord, squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos) });
					queued_chunks_set.insert(chunk_coord);
				}
			}
		}
	}
}

Chunk* World::genChunk(ChunkCoordinates coordinates) {
	auto gen_start = std::chrono::high_resolution_clock::now();

	Chunk* chunk = new Chunk(coordinates);
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

void World::update(StreamTarget target) {
	// profiling ---------
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::Begin("Render Info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Avg Mesh Time: %fms", total_mesh_time / num_meshed);
	ImGui::Text("Avg Gen Time: %fms", total_gen_time / num_generated);
	ImGui::Text("Avg Chunk Vertex Count: %f", (float)total_vertices_generated / num_meshed);
	ImGui::End();


	// -------------------
	streamTerrain(target);
	// TODO: remove magic number for budget(s)
	for (int i = 0; i < 4; ++i) {
		if (queued_chunks.empty()) {
			break;
		}

		int chunk_size = Chunk::CHUNK_SIZE;
		ChunkGenRequest top = queued_chunks.top();

		queued_chunks.pop();

		// skip chunk if already loaded
		if (loaded_chunks.contains(top.coordinates)) {
			queued_chunks_set.erase(top.coordinates);
			continue;
		}

		// generate chunk
		loaded_chunks[top.coordinates] = genChunk(top.coordinates);
		queued_chunks_set.erase(top.coordinates);

		// mark chunk neighbors as dirty to remesh, as chunk border now contains non-air blocks
		if (loaded_chunks.contains({ top.coordinates.x + 1, top.coordinates.y, top.coordinates.z }))
			loaded_chunks[{top.coordinates.x + 1, top.coordinates.y, top.coordinates.z}]->dirty = true;

		if (loaded_chunks.contains({ top.coordinates.x - 1, top.coordinates.y, top.coordinates.z }))		
			loaded_chunks[{top.coordinates.x - 1, top.coordinates.y, top.coordinates.z}]->dirty = true;

		if (loaded_chunks.contains({ top.coordinates.x, top.coordinates.y + 1, top.coordinates.z }))
			loaded_chunks[{top.coordinates.x, top.coordinates.y + 1, top.coordinates.z}]->dirty = true;

		if (loaded_chunks.contains({ top.coordinates.x, top.coordinates.y - 1, top.coordinates.z }))
			loaded_chunks[{top.coordinates.x, top.coordinates.y - 1, top.coordinates.z}]->dirty = true;

		if (loaded_chunks.contains({ top.coordinates.x, top.coordinates.y, top.coordinates.z + 1}))
			loaded_chunks[{top.coordinates.x, top.coordinates.y, top.coordinates.z + 1}]->dirty = true;

		if (loaded_chunks.contains({ top.coordinates.x, top.coordinates.y, top.coordinates.z - 1 }))
			loaded_chunks[{top.coordinates.x, top.coordinates.y, top.coordinates.z - 1}]->dirty = true;
	}
	
	for (Chunk* chunk : getVisibleChunks(target)) {
		for (int i = 0; i < 40; ++i) {
			if (chunk->dirty) {
				auto mesh_start = std::chrono::high_resolution_clock::now();

				std::pair<std::vector<Vertex>, std::vector<GLuint>> mesh_data = chunk_mesher.buildGreedyMesh(getSurroundingChunks(chunk));
				chunk->chunk_mesh = Mesh(mesh_data.first, mesh_data.second);
				chunk->dirty = false;

				auto mesh_end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration<double, std::milli>(mesh_end - mesh_start);
				total_mesh_time += duration;
				num_meshed++;
				total_vertices_generated += mesh_data.first.size();
			}

		}
		
	}
}

ChunkGroup World::getSurroundingChunks(Chunk* chunk) {
	ChunkGroup group;
	group.main = chunk;

	if (loaded_chunks.contains({ chunk->getChunkPosition().x - 1, chunk->getChunkPosition().y, chunk->getChunkPosition().z })) {
		group.left = loaded_chunks[{chunk->getChunkPosition().x - 1, chunk->getChunkPosition().y, chunk->getChunkPosition().z}];
	}
	if (loaded_chunks.contains({ chunk->getChunkPosition().x + 1, chunk->getChunkPosition().y, chunk->getChunkPosition().z })) {
		group.right = loaded_chunks[{chunk->getChunkPosition().x + 1, chunk->getChunkPosition().y, chunk->getChunkPosition().z}];
	}
	if (loaded_chunks.contains({ chunk->getChunkPosition().x, chunk->getChunkPosition().y - 1, chunk->getChunkPosition().z })) {
		group.bottom = loaded_chunks[{chunk->getChunkPosition().x, chunk->getChunkPosition().y - 1, chunk->getChunkPosition().z}];
	}
	if (loaded_chunks.contains({ chunk->getChunkPosition().x, chunk->getChunkPosition().y + 1, chunk->getChunkPosition().z })) {
		group.top = loaded_chunks[{chunk->getChunkPosition().x, chunk->getChunkPosition().y + 1, chunk->getChunkPosition().z}];
	}
	if (loaded_chunks.contains({ chunk->getChunkPosition().x, chunk->getChunkPosition().y, chunk->getChunkPosition().z - 1 })) {
		group.back = loaded_chunks[{chunk->getChunkPosition().x, chunk->getChunkPosition().y, chunk->getChunkPosition().z - 1 }];
	}
	if (loaded_chunks.contains({ chunk->getChunkPosition().x, chunk->getChunkPosition().y, chunk->getChunkPosition().z + 1 })) {
		group.front = loaded_chunks[{chunk->getChunkPosition().x, chunk->getChunkPosition().y, chunk->getChunkPosition().z + 1 }];
	}

	return group;
}


float World::squaredDistance(glm::vec3 a, glm::vec3 b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return (dx * dx) + (dy * dy) + (dz * dz);
}

