#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/Mesh.hpp"
#include "../include/Camera.hpp"

#include <vector>
#include <cmath>

World::World(BlockMeshingContext context) : last_streamed_chunk_coord({ 0, 0, 0 }), last_radius(0), context(&context) {
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.02);
}

std::vector<Chunk*> World::getVisibleChunks(Camera &camera) {
	std::vector<Chunk*> visible_chunks;

	// TODO: this just returns *all* chunks right now. not exactly efficient
	for (auto& chunk : coords_to_chunk) {
		visible_chunks.push_back(chunk.second);
	}

	return visible_chunks;
}

void World::streamTerrain(StreamTarget target) {
	int chunk_size = Chunk::getChunkSize();

	// get target position
	Coordinates current_chunk_coord;
	current_chunk_coord.x = std::floor(static_cast<double>(target.pos.x) / chunk_size);
	current_chunk_coord.y = std::floor(static_cast<double>(target.pos.y) / chunk_size);
	current_chunk_coord.z = std::floor(static_cast<double>(target.pos.z) / chunk_size);

	if (current_chunk_coord == last_streamed_chunk_coord && target.load_radius == last_radius) {
		return;
	}

	last_streamed_chunk_coord = current_chunk_coord;
	last_radius = target.load_radius;

	// create new chunk(s) at/around that position
	for (int x = -target.load_radius; x < target.load_radius; ++x) {
		for (int z = -target.load_radius; z < target.load_radius; ++z) {
			for (int y = -target.load_radius; y < target.load_radius; ++y) {
				// get coordinates of surrounding chunks
				Coordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				if (coords_to_chunk.contains(chunk_coord)) {
					continue;
				}

				// add new chunk to hashmap
				coords_to_chunk[chunk_coord] = genChunk(chunk_coord, 1);
			}
		}
	}
}

Chunk* World::genChunk(Coordinates coordinates, int seed) {
	Chunk* chunk = new Chunk(coordinates);
	int chunk_size = Chunk::getChunkSize();

	for (int x = 0; x < chunk_size; ++x) {
		for (int y = 0; y < chunk_size; y++) {
			for (int z = 0; z < chunk_size; z++) {
				int height = noise.GetNoise(static_cast<float>(coordinates.x * chunk_size + x), static_cast<float>(coordinates.z * chunk_size + z)) * 50;

				if ((coordinates.y * chunk_size + y) < height) {
					chunk->positions[x][y][z] = 1;
				}
				if ((coordinates.y * chunk_size + y) == height) {
					chunk->positions[x][y][z] = 3;
				}
			}
		}
	}
	
	return chunk;
}

void World::update(StreamTarget target) {
	streamTerrain(target);
	
	for (auto& chunk : coords_to_chunk) {
		if (chunk.second->dirty) {
			MeshData mesh_data = chunk_mesher.build(*(chunk.second), *context);
			chunk.second->chunk_mesh = Mesh(mesh_data.vertices);
			chunk.second->dirty = false;
		}
	}
}

