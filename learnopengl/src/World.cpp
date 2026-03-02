#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/Mesh.hpp"
#include "../include/Camera.hpp"

#include <iostream>
#include <vector>

std::vector<Chunk*> World::getVisibleChunks(Camera &camera) {
	std::vector<Chunk*> visible_chunks;

	// TODO: this just returns *all* chunks right now. not exactly efficient
	for (auto& chunk : coords_to_chunk) {
		visible_chunks.push_back(chunk.second);
	}

	return visible_chunks;
}

void World::genTerrain(StreamTarget target) {
	int chunk_size = Chunk::getChunkSize();

	// get target position
	Coordinates current_chunk_coord;
	current_chunk_coord.x = target.pos.x / chunk_size;
	current_chunk_coord.y = target.pos.y / chunk_size;
	current_chunk_coord.z = target.pos.z / chunk_size;

	if (current_chunk_coord == last_streamed_chunk_coord && target.load_radius == last_radius) {
		return;
	}

	last_streamed_chunk_coord = current_chunk_coord;
	last_radius = target.load_radius;

	// create new chunk(s) at/around that position
	for (int x = -target.load_radius; x < target.load_radius; ++x) {
		for (int z = -target.load_radius; z < target.load_radius; ++z) {

			// get coordinates of surrounding chunks
			Coordinates chunk_coord;
			chunk_coord.x = current_chunk_coord.x + x;
			chunk_coord.y = 0;
			chunk_coord.z = current_chunk_coord.z + z;

			// add new chunk to hashmap
			if (!coords_to_chunk.contains(chunk_coord)) {
				Chunk* chunk = new Chunk(chunk_coord);

				// add blocks to chunk (hardcoded, based on noisemap, whatever)
				for (int i = 0; i < chunk_size; ++i) {
					for (int j = 0; j < chunk_size; j++) {
						chunk->positions[i][0][j] = 3;
					}
				}

				coords_to_chunk[chunk_coord] = chunk;
			}
		}
	}
}

void World::update(StreamTarget target) {
	genTerrain(target);
	
	for (auto& chunk : coords_to_chunk) {
		if (chunk.second->dirty) {
			// std::cout << chunk.second->chunk_position.x << std::endl;

			MeshData mesh_data = chunk_mesher.build(*(chunk.second));
			chunk.second->chunk_mesh = Mesh(mesh_data.vertices);
			chunk.second->dirty = false;
		}
	}
}

