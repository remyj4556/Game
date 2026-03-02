#include "../include/World.hpp"

void World::genTestChunk() {
	// TODO: testing dynamic chunk generation 
	int num_chunks = 10;
	int chunk_size = Chunk::getChunkSize();

	for (int i = 0; i < num_chunks; ++i) {
		Coordinates pos = { i, 0, i };
		Chunk* chunk = new Chunk;
		chunk->chunk_position = pos;

		// fill with air 
		for (int x = 0; x < chunk_size; ++x) {
			for (int y = 0; y < chunk_size; ++y) {
				for (int z = 0; z < chunk_size; ++z) {
					chunk->positions[x][y][z] = 0;
				}
			}
		}

		// place different blocks
		chunk->positions[5][5][5] = 1;
		chunk->positions[6][5][5] = 2;
		chunk->positions[7][5][5] = 3;

		for (int x = 0; x < 3; ++x) {
			for (int y = 0; y < 3; ++y) {
				for (int z = 0; z < 3; ++z) {
					chunk->positions[10 + x][10 + y][10 + z] = 4;
				}
			}
		}

		for (int x = 0; x < 10; ++x) {
			for (int z = 0; z < 10; ++z) {
				chunk->positions[x][0][z] = 3;
			}
		}

		coords_to_chunk[pos] = chunk;
	}

	// create the meshes (note: we would typically create this mesh then continually update it in the game loop whenever blocks are changed/broken/placed)
	for (auto& chunk : coords_to_chunk) {
		std::cout << chunk.second->chunk_position.x << std::endl;
		chunk.second->updateChunkMesh();
	}
}


std::vector<Chunk*> World::getVisibleChunks(Camera& camera) {
	std::vector<Chunk*> visible_chunks;

	// TODO: this just returns *all* chunks right now. not exactly efficient
	for (auto& chunk : coords_to_chunk) {
		visible_chunks.push_back(chunk.second);
	}

	return visible_chunks;
}