#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include "ChunkMesher.hpp"
#include "ChunkCoordinates.hpp"
#include "Camera.hpp"
#include "BlockMeshingContext.hpp"
#include <unordered_map>
#include <vector>

struct StreamTarget {
	glm::vec3 pos;
	int load_radius;
};

class World {
	private:
		Coordinates last_streamed_chunk_coord;
		int last_radius;

		ChunkMesher chunk_mesher;
		const BlockMeshingContext* context;
		std::unordered_map<Coordinates, Chunk*, CoordinatesHash> coords_to_chunk;
		void genTerrain(StreamTarget target);


	public:
		World(BlockMeshingContext context);
		std::vector<Chunk*> getVisibleChunks(Camera &camera);
		void update(StreamTarget target);
};

#endif