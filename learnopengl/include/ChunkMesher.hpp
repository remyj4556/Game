#ifndef CHUNKMESHER_HPP
#define CHUNKMESHER_HPP

#include <functional>
#include <vector>
#include "Vertex.hpp"
#include "Chunk.hpp"
#include "Block.hpp"
#include "BlockMeshingContext.hpp"
#include "WorldCoordinates.hpp"

struct MeshData {
	std::vector<Vertex> vertices;
	//std::vector<uint32_t> indices;
};

// Chunk Mesher basically acts as just a utility function, taking in a Chunk and creating its mesh.
// TODO: this will need to eventually have neighboring chunks passed in, to cull chunk borders
class ChunkMesher {
	public:
		MeshData build(Chunk &chunk, BlockMeshingContext context, std::function<Block(WorldCoordinates)> blockAtWorldPos);
};

#endif