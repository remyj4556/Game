#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/BlockRegistry.hpp"
#include "../include/Mesh.hpp"

#include <vector>
#include <cmath>

World::World(BlockMeshingContext context) : last_streamed_chunk_coord({ 0, 0, 0 }), last_radius(0), context(context) {
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.02);
}

// TODO: test this function when implementing block placement. should not update meshes of chunks that are
// outside target.load_radius
std::vector<Chunk*> World::getVisibleChunks(StreamTarget target) {
	std::vector<Chunk*> visible_chunks;

	int chunk_size = Chunk::getChunkSize();

	// this returns all chunks that are within the target's load radius
	for (auto& chunk : loaded_chunks) {
		int chunk_x = chunk.second->chunk_position.x * chunk_size;
		int chunk_y = chunk.second->chunk_position.y * chunk_size;
		int chunk_z = chunk.second->chunk_position.z * chunk_size;

		if (chunk_x > target.pos.x + target.load_radius ||
			chunk_x < target.pos.x - target.load_radius ||
			chunk_y > target.pos.y + target.load_radius ||
			chunk_y < target.pos.y - target.load_radius ||
			chunk_z > target.pos.z + target.load_radius ||
			chunk_z < target.pos.z - target.load_radius) {

			visible_chunks.push_back(chunk.second);
		}
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


	for (int x = -target.load_radius; x <= target.load_radius; ++x) {
		for (int z = -target.load_radius; z <= target.load_radius; ++z) {
			for (int y = -target.load_radius; y <= target.load_radius; ++y) {
				// get coordinates of surrounding chunks
				Coordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to load a sphere
				if ((x * x + y * y + z * z) > (target.load_radius * target.load_radius)) {
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

				// otherwise add chunk to queue to load/generate
				

				if (!loaded_chunks.contains(chunk_coord) && !queued_chunks_set.contains(chunk_coord)) {
					//std::cout << "QUEUING: " << chunk_coord << "\n";
					queued_chunks.push({ chunk_coord, squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos) });
					queued_chunks_set.insert(chunk_coord);
				}
			}
		}
	}
}

Chunk* World::genChunk(Coordinates coordinates) {
	Chunk* chunk = new Chunk(coordinates);
	int chunk_size = Chunk::getChunkSize();

	for (int x = 0; x < chunk_size; ++x) {
		for (int z = 0; z < chunk_size; z++) {
			int height = noise.GetNoise(static_cast<float>(coordinates.x * chunk_size + x), static_cast<float>(coordinates.z * chunk_size + z)) * 50;

			for (int y = 0; y < chunk_size; y++) {
				if ((coordinates.y * chunk_size + y) < height) {
					chunk->positions[x][y][z].id = 1;
				}
				if ((coordinates.y * chunk_size + y) == height) {
					chunk->positions[x][y][z].id = 3;
				}
			}
		}
	}
	
	return chunk;
}

void World::update(StreamTarget target) {
	streamTerrain(target);

	std::cout << queued_chunks.size() << " " << loaded_chunks.size() << "\n";

	// generate some number (say, 4 right now) of chunks per frame
	for (int i = 0; i < 4; ++i) {
		if (queued_chunks.empty()) {
			break;
		}

		int chunk_size = Chunk::getChunkSize();
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
	}
	

	// assign a budget per frame here, that only pulls from *loaded* chunks
	for (Chunk* chunk : getVisibleChunks(target)) {
		if (chunk->dirty) { // check if chunk is actually generated here
			MeshData mesh_data = chunk_mesher.build(*chunk, context);
			chunk->chunk_mesh = Mesh(mesh_data.vertices);
			chunk->dirty = false;
		}
	}
}


float World::squaredDistance(glm::vec3 a, glm::vec3 b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return (dx * dx) + (dy * dy) + (dz * dz);
}

