#include "../include/World.hpp"
#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/WorldCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/Mesh.hpp"

#include <vector>
#include <cmath>
#include <cstdint>

World::World(BlockMeshingContext context) : last_streamed_chunk_coord({ 0, 0, 0 }), last_radius(0), context(context) {
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(0.02);
}

WorldCoordinates World::chunkToWorld(ChunkCoordinates chunk_coords) {
	int chunk_size = Chunk::getChunkSize();

	WorldCoordinates c;
	c.x = chunk_coords.x * chunk_size;
	c.y = chunk_coords.y * chunk_size;
	c.z = chunk_coords.z * chunk_size;

	return c;
}

ChunkCoordinates World::worldToChunk(WorldCoordinates world_coords) {
	int chunk_size = Chunk::getChunkSize();

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
	int chunk_size = Chunk::getChunkSize();

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

// TODO: test this function when implementing block placement. should not update meshes of chunks that are
// outside target.load_radius
std::vector<Chunk*> World::getVisibleChunks(StreamTarget target) {
	std::vector<Chunk*> visible_chunks;

	int chunk_size = Chunk::getChunkSize();

	// this returns all chunks that are within the target's load radius
	for (auto& chunk : loaded_chunks) {
		WorldCoordinates world_coords = chunkToWorld(chunk.second->getChunkPosition());

		if (world_coords.x > target.pos.x + target.load_radius ||
			world_coords.x < target.pos.x - target.load_radius ||
			world_coords.y > target.pos.y + target.load_radius ||
			world_coords.y < target.pos.y - target.load_radius ||
			world_coords.z > target.pos.z + target.load_radius ||
			world_coords.z < target.pos.z - target.load_radius) {
			
			visible_chunks.push_back(chunk.second);
		}
	}

	return visible_chunks;
}

void World::streamTerrain(StreamTarget target) {
	int chunk_size = Chunk::getChunkSize();

	// get target position
	ChunkCoordinates current_chunk_coord;
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
				ChunkCoordinates chunk_coord;
				chunk_coord.x = current_chunk_coord.x + x;
				chunk_coord.y = current_chunk_coord.y + y;
				chunk_coord.z = current_chunk_coord.z + z;

				// skip "corners" to load a sphere
				if ((x * x + y * y + z * z) > (target.load_radius * target.load_radius)) {
					continue;
				}

				// add chunk to queue to load/generate
				if (!loaded_chunks.contains(chunk_coord) && !queued_chunks_set.contains(chunk_coord)) {
					//std::cout << "QUEUING: " << chunk_coord << "\n";
					queued_chunks.push({ chunk_coord, squaredDistance({ chunk_coord.x * chunk_size, chunk_coord.y * chunk_size, chunk_coord.z * chunk_size }, target.pos) });
					queued_chunks_set.insert(chunk_coord);
				}
			}
		}
	}
}

Chunk* World::genChunk(ChunkCoordinates coordinates) {
	Chunk* chunk = new Chunk(coordinates);
	int chunk_size = Chunk::getChunkSize();

	for (uint8_t x = 0; x < chunk_size; ++x) {
		for (uint8_t z = 0; z < chunk_size; z++) {
			int height = noise.GetNoise(static_cast<float>(coordinates.x * chunk_size + x), static_cast<float>(coordinates.z * chunk_size + z)) * 50;

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

		// mark chunk neighbors as dirty to remesh, as chunk border now contains non-air blocks
		/*
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

		*/
	}
	

	// TODO: assign a budget per frame here as well
	for (Chunk* chunk : getVisibleChunks(target)) {
		if (chunk->dirty) {
			MeshData mesh_data = chunk_mesher.build(*chunk, context, [this](WorldCoordinates world_coords) { return this->blockAtWorldPos(world_coords); });
			chunk->chunk_mesh = Mesh(mesh_data.vertices);
			chunk->dirty = true;
		}
	}
}


float World::squaredDistance(glm::vec3 a, glm::vec3 b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;

	return (dx * dx) + (dy * dy) + (dz * dz);
}

